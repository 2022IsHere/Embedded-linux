#!/bin/bash

RED='\033[0;31m'
NC='\033[0m' # No Color

username=pi
passwd=raspberry

while getopts ":t:p:u:" opt; do
  case $opt in
    t) ipaddr="$OPTARG"
    ;;
    p) passwd="$OPTARG"
    ;;
    u) username="$OPTARG"
    ;;
    \?) echo "Invalid option -$OPTARG" >&2
    exit 1
    ;;
  esac

  case $OPTARG in
    -*) echo "Option $opt needs a valid argument"
    exit 1
    ;;
  esac
done

printf "Using parameters:\n"
printf "Raspi target is %s\n" "$ipaddr"
printf "Raspi username is %s\n" "$username"
printf "Raspi password is %s\n" "$passwd"

#
# Check raspi connection
#
printf "Checking pi connectivity with nmap... "
connectivity_status=$(nmap $ipaddr | grep -E "22/tcp +open +ssh")
if [ -z "$connectivity_status" ]; then
    echo -e "\n${RED}Error: Pi is not connectable!!!"
    echo "Try testing with 'nmap $ipaddr'"
    exit 1;
else
    printf "OK, Pi is up with ssh port open\n"
fi


#
# Check ssh login
# If both passwd and publickey exist, then publickey is used ==> do not know passwd validity.
# See output in raspi /var/log/auth.log to see which auth method was accepted.
#
printf "Checking pi ssh login... "
pi_auth_log=$(sshpass -p $passwd ssh -o "StrictHostKeyChecking no" $username@$ipaddr 'cat /var/log/auth.log | grep "Accepted" | tail -1')
if test "$?" != "0"; then
    echo -e "${RED}Error: failed to login to raspi!!!"
    echo "Check username and passwd."
    exit 1;
fi
pi_auth_pass=$(echo $pi_auth_log | grep "Accepted password")
pi_auth_keys=$(echo $pi_auth_log | grep "Accepted publickey")
#printf "\npass:  %q\n" "$pi_auth_pass"
#printf "\nkeys:  %q\n" "$pi_auth_keys"
if [ -n "$pi_auth_pass" ]; then
    printf "OK, login with password successful.\n"
elif [ -n "$pi_auth_keys" ]; then
    printf "OK, login with publickey successful.\n"
else
    echo -e "${RED}Error: unknown login result (not password or publickey)!!!"
    echo "Check login details."
    exit 1;
fi

#
# Check raspi version
#

pi_osversion=$(sshpass -p $passwd ssh $username@$ipaddr 'cat /etc/os-release')
if test "$?" != "0"; then
    echo -e "${RED}Error: failed to run cmd on raspi!!!"
    exit 1;
fi
#printf "Pi OS version is %s\n" "$pi_osversion"

printf "Checking pi OS name... "
pi_os_id=$(echo $pi_osversion | tr ' ' '\n' |  grep '^ID=')
if [ "$pi_os_id" = "ID=raspbian" ]; then
    printf "OK, Pi OS id is %s\n" "$pi_os_id"
else
    echo -e "${RED}Error: Pi OS is not Raspbian!!!"
    printf "     Pi OS id is %s\n" "$pi_os_id"
fi

printf "Checking pi OS version ... "
pi_os_version_codename=$(echo $pi_osversion | tr ' ' '\n' |  grep VERSION_CODENAME)
if [ "$pi_os_version_codename" = "VERSION_CODENAME=bullseye" ]; then
    printf "OK, Pi OS codename is %s\n" "$pi_os_version_codename"
else
    echo -e "${RED}Error: Pi OS version is not bullseye!!!"
    printf "      Pi OS codename is %s\n" "$pi_os_version_codename"
fi

#
# Get pi hostname
#
printf "Checking pi IP and hostname ... "
pi_hostname=$(sshpass -p $passwd ssh $username@$ipaddr hostname)
if test "$?" != "0"; then
    echo -e "${RED}Error: failed to run cmd on raspi!!!"
    exit 1;
fi
pi_ip=$(sshpass -p $passwd ssh $username@$ipaddr ip -4 -o address show dev eth0 | awk '{print $4}')
if test "$?" != "0"; then
    echo -e "${RED}Error: failed to run cmd on raspi!!!"
    exit 1;
fi
printf "OK, Pi (%s) hostname is %s\n" "$pi_ip" "$pi_hostname"


#
# Set up ssh keys
#
if [ -n "$pi_auth_pass" ]; then
    printf "Checking ssh keys on host VM..."
    FILE=~/.ssh/id_rsa.pub
    if test -f "$FILE"; then
        echo "Using existing key in $FILE."
    else
        printf "Generating host VM ssh keys..."
        ssh-keygen -t rsa -b 2048 -f /home/student/.ssh/id_rsa -q -N ""
        echo "OK"
    fi
    printf "Checking ssh keys on Raspi..."
    # https://superuser.com/questions/264384/ssh-copy-id-and-duplicates-in-authorized-keys
    cat ~/.ssh/id_rsa.pub | sshpass -p $passwd ssh $username@$ipaddr 'mkdir -pm 0700 ~/.ssh &&
        touch ~/.ssh/authorized_keys && 
        chmod 600 ~/.ssh/authorized_keys &&
        while read -r ktype key comment; do
            if ! (grep -Fw "$ktype $key" ~/.ssh/authorized_keys | grep -qsvF "^#"); then
                echo "Adding host key to raspi ~/.ssh/authorized_keys"
                echo "$ktype $key $comment" >> ~/.ssh/authorized_keys
            else
                echo "Host key already listed in ~/.ssh/authorized_keys"
           fi
        done'
fi

#
# Install gdbserver on raspi
#
printf "Install gdbserver to raspi ... "
scp -q ~/opt/x-tools/armv6-rpi-linux-gnueabihf/armv6-rpi-linux-gnueabihf/debug-root/usr/bin/gdbserver $username@$ipaddr:~
ssh $username@$ipaddr sudo mv gdbserver /usr/local/bin
printf "OK\n"

#
# Create raspi shortcut for ssh
#
printf "Create ssh config for $pi_hostname ... "
touch ~/.ssh/config
sshconf_exist=$(cat ~/.ssh/config | grep "HostName $pi_hostname.local")
if [ -n "$sshconf_exist" ]; then
    printf "OK, config was there already.\n"
else
    cat >> ~/.ssh/config << EOF
Host rpi
    HostName $ipaddr
    User $username
EOF
    printf "OK, new config created.\n"
fi

#
# Update pi
#
printf "Update pi ... "
# get rid of locale warnings
ssh $username@$ipaddr "sudo bash -c 'cat >> /etc/environment << EOF
LC_ALL=en_GB.UTF-8
LANG=en_GB.UTF-8
EOF'"

pi_hostname=$(ssh $username@$ipaddr 'DEBIAN_FRONTEND=noninteractive sudo apt-get update && sudo apt-get -y upgrade')
if test "$?" != "0"; then
    echo -e "${RED}Error: failed to run update cmd on raspi!!!"
    exit 1;
fi
printf "OK\n"


#
# Update VM cross chroot
#
printf "Update cross rootfs on VM ... "
sudo sbuild-apt rpizero-bullseye-armhf apt-get update >> sbuild-apt.log
sudo sbuild-apt rpizero-bullseye-armhf apt-get upgrade >> sbuild-apt.log
printf "OK\n"
