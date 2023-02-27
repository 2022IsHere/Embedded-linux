# Lab 8. Setting up an embedded web server on RasPi

**NOTE:** Please watch the two related video lectures (from Teams) before starting the lab!   

In many cases the embedded device does not have any user interface at all. However, the system needs to provide some kind of access for the local user, and in many cases the local user has no skills for command-line interface and ssh, but needs a simplified graphical user interface.  
So in these cases the system is not intended to connect to internet for remote operation, but only provide access for local user. Remote operation would require stricter security and support for multiple simultanous users.  

In this lab you'll set up web server for a single local user.  

## Step 1: Setting up a web server on raspi

As always, you'll want to start with an up-to-date RasPi system. Open up a ssh connection
```
pi@raspberrypi:~ $ sudo apt update
pi@raspberrypi:~ $ sudo apt upgrade
pi@raspberrypi:~ $ sudo apt install lighttpd
```
In order to keep cross-development environment synced, you should replicate this on VM debug environment. Check Lab1 instructions for this.

The installation will get everything done: install necessary files with basic configuration, start the service, and enable the service so that it will start after next reboot as well.
To verify this 
```
pi@raspberrypi:~ $ sudo systemctl status lighttpd.service
```
Status should contain states <b>loaded, enabled, running</b>

You should be able to see the default placeholder page with browser at raspi IP:  http://xx.xx.xx.xx
Note that this must be http, so if browser forces to use https there will be no connection.

QUESTION #1: From the VM, get default page content to file using ``curl http://x.x.x.x > result1.html``, put it into lab8/results repo path, commit and push. 

If you have problems here, jump to [troubleshooting](#troubleshooting)

## Step 2: Understanding server configuration

The server main config file is named `lighttpd.conf`. Find it using `sudo find / -name filename`, and edit the file as root user
```
pi@raspberrypi:~ $ sudo nano /path-to-config/lighttpd.conf
```
It is always a recommended practise to make sure you understand how the setup behaves when something goes wrong. So, the first step before making any more complex changes to configuration, we make an intentional error there and check that we see proper ewrror messages.
So, add the following line anywhere in config file:
```
jkcergfserjkctgheuy rhfesyc
```
save file, and reload service with new config:
```
pi@raspberrypi:~ $ sudo service lighttpd force-reload
```
Well, where is our error message? Try checking out following things:
```
pi@raspberrypi:~ $ sudo systemctl status lighttpd.service
pi@raspberrypi:~ $ sudo tail -20 /var/log/lighttpd/error.log
pi@raspberrypi:~ $ sudo tail -20 /var/log/syslog
```
QUESTION #2: Where did you find information on which file and line your error was? Get the relevant error message to file result2, put it into lab8/results repo path, commit and push.

Finally, remove the offending line from config, save file and start service. Check that it runs normally again. (Hint: force-reload requires the service is running, so it may fail. You need to <b>start</b> the service.
> There is a nice summary on systemctl features: https://www.digitalocean.com/community/tutorials/how-to-use-systemctl-to-manage-systemd-services-and-units (and of course chapter 18.10 in NDG Linux course).


## Step 3: Configuring server logging

You will want to have maximal visibility into your system, so it is necessary to find out how to add more logging.

lighttpd has modular design, so for every new feature you need to add a new module. This makes sense securitywise: if you do not need some feature, then you do not need to suffer a wider attack surface.

> A web server access log is a list of all requests for individual files that people or bots have requested from a website. 

```
pi@raspberrypi:~ $ sudo lighty-enable-mod accesslog
pi@raspberrypi:~ $ sudo service lighttpd force-reload
pi@raspberrypi:~ $ sudo systemctl status lighttpd.service
```
Next you need to find out where is that access log. You should start with reading the main config file in (Raspi) /etc/lighttpd/lighttpd.conf and also reading through the included config files. Then,
```
pi@raspberrypi:~ $ sudo tail -f [path to access file]
```
This command shows the tail of the log file, and updates when new log rows appear. You can now refresh your browser and you should see new events in access log.  
Next, we want more verbosity in the logs. Add the following lines to end of lighttpd main config file
```
debug.log-request-header = "enable" 
debug.log-response-header = "enable" 
debug.log-request-handling = "enable" 
debug.log-file-not-found = "enable" 
debug.log-condition-handling = "enable" 
debug.log-timeouts = "enable" 
```
and reload the configuration. The verbose logs will go to error.log file (...even though if there were no errors with the requests...). Repeat page refresh in browser and check that error.log has verbose content, like
```
...
2022-02-03 05:09:54: (response.c.658) -- logical -> physical 
2022-02-03 05:09:54: (response.c.659) Doc-Root     : /var/www/html 
2022-02-03 05:09:54: (response.c.660) Basedir      : /var/www/html 
2022-02-03 05:09:54: (response.c.661) Rel-Path     : /orig/ 
2022-02-03 05:09:54: (response.c.662) Path         : /var/www/html/orig/ 
2022-02-03 05:09:54: (response.c.674) -- handling physical path 
2022-02-03 05:09:54: (response.c.675) Path         : /var/www/html/orig/ 
2022-02-03 05:09:54: (response.c.682) -- handling subrequest 
2022-02-03 05:09:54: (response.c.683) Path         : /var/www/html/orig/ 
2022-02-03 05:09:54: (response.c.684) URI          : /orig/ 
2022-02-03 05:09:54: (response.c.685) Pathinfo     :  
...
```

QUESTION #3: Copypaste a snippet of log output showing verbose content to file lab8/result3.log, commit and push.

## Step 4: Configuring SSL

Next we want enable https connection. For that the server needs to have SSL keys and a certificate.  
> A proper <b>SSL/TLS certificate</b> would require a public IP and a domain name. As we are working in a private network, we can only have a <b>self signed certificate</b>. Browsers are not happy with self-signed certificates (securitywise, they do not certify much, because anyone can create one).
```
pi@raspberrypi:~ $ sudo mkdir /etc/lighttpd/certs
pi@raspberrypi:~ $ sudo openssl req -new -x509 -keyout /etc/lighttpd/certs/lighttpd.pem -out /etc/lighttpd/certs/lighttpd.pem -days 365 -nodes
pi@raspberrypi:~ $ sudo chmod 400 /etc/lighttpd/certs/lighttpd.pem
```
It really doesn't matter what you fill in for the interactive questions, because we are using raspi with IP address only, not hostname.

> Web servers are very sensitive to access permissions for the certificate key files. If you set too wide permissions for the keys, then server does not accept your certificate.

Then we enable ssl for lighttpd. You need to make these two changes to main config file:
```
server.modules = (
	"mod_indexfile",
	"mod_access",
	"mod_alias",
	"mod_redirect",
	"mod_openssl",      # <-- add this module
)

...

# add these lines to end of file
$SERVER["socket"] == ":443" {
  ssl.engine = "enable" 
  ssl.pemfile = "/etc/lighttpd/certs/lighttpd.pem" 
}

```
Reload config and check that you can get https connection (you'll need to navigate past "insecure certificate" warnings in browser).  
For more options, see https://redmine.lighttpd.net/projects/1/wiki/HowToSimpleSSL

QUESTION #4:  From the VM, get default page content to file using ``curl https://x.x.x.x > result4.html``, put it into lab8/results repo path, commit and push. 

## Step 5: Set up your own static pages

Before making any changes:
	- where is current placeholder page located (== document root)? Hint: READ the placeholder file content! (You have it conveniently stored in your repo as well.)
	- who is the owner/group for the current index file (placeholder page)? Web server needs to read access for the file.

To Do:
  - in document root create subfolder "orig", move the existing placeholder file there and check that browser finds it https://xx.xx.xx.xx/orig (and 403 from doc root)
  - create new index.html file with content X, check with browser. For content, see HTML tutorials in https://www.w3schools.com/html/html_examples.asp

QUESTION #5: From the VM, get default page content to file using ``curl https://x.x.x.x/orig > result5.html``, put it into lab8/results repo path, commit and push. 


## Troubleshooting

#### lighttpd service not starting

```
pi@raspberrypi:~ $ sudo systemctl status lighttpd.service
```
Does your lighttpd service show green? Loaded, running & enabled?

If not, try starting it, and then check all logs to find first error message with line & column numbers:
```
pi@raspberrypi:~ $ sudo systemctl start lighttpd.service
pi@raspberrypi:~ $ sudo systemctl status lighttpd.service
pi@raspberrypi:~ $ sudo tail -20 /var/log/lighttpd/error.log
pi@raspberrypi:~ $ sudo tail -20 /var/log/syslog
pi@raspberrypi:~ $ journalctl -u lighttpd.service
```
> There is a nice summary on systemctl features: https://www.digitalocean.com/community/tutorials/how-to-use-systemctl-to-manage-systemd-services-and-units (and of course chapter 18.10 in NDG Linux course).


#### Browser times out: "The connection has timed out" or "This site can’t be reached"

1. Check that lighttpd service is up and running (as above)
2. Check that your browser is trying to connect the correct IP address
    - raspi ip as shown in raspi CLI `ip a show eth0` and look at IPv4 address there
3. Check that your browser is trying to use a protocol that i supported by your lighttpd configuration (in first steps http only)
    - to see if it is a browser broblem, try form CLI as well (both VM/Linux and host/Windows apply)   
    `curl http://xx.xx.xx.xx`  
    `curl --insecure https://xx.xx.xx.xx`  
    - if browser problem, check your browser settings if they enforce https
4. Check that your browser and raspi are in same network
    - in both raspi and Ubuntu VM `ip a` shows IP
    - Ubuntu VM must use "bridged" mode network interface, and that physical parent interface must be connected to the same network where raspi is
    - your VM network adapter state is "Cable: Connected" (icons on bottom right in VM window)
    - try ping both ways
5.Check that there are no firewall settings that block connections
    - in raspi and Ub untu VM `sudo iptables -L`
6. Check that all devices have healthy network stacks
    - in all three systems (raspi, VM, laptop host OS), try pinging the default gateway. 
        - in Linux default GW IP should be found using `ip r` "default via x.x.x.x",  
          in Windows `ipconfig /all` and/or `route print` 
        - in elelab network default GW should be 172.27.224.0
        - in emblab network default GW should be 172.27.240.0
    - if any device cannot see default GW, then it has the problem
        - raspi: try `sudo service networking restart` and if no help, then try reboot
        - Ubuntu VM: `sudo netplan apply` and if no help, then try reboot
        - Windows: shutdown VM, reboot Windows.

#### Browser requests are not shown in lighttpd logs

Check that your configs have access logs enabled, the log directory exists, and that the directory and log files are owned by user:group `www-data:www-data` with relevant permissions:
```
pi@raspberrypi:/home $ sudo ls -ld /var/log/lighttpd
drwxr-x--- 2 www-data www-data 4096 Feb  1 14:05 /var/log/lighttpd

pi@raspberrypi:/home $ sudo ls -l /var/log/lighttpd/error.log
-rw-r--r-- 1 www-data www-data 15010919 Feb  4 07:37 /var/log/lighttpd/error.log
```



Enable all relevant verbosity in config
```
debug.log-request-header = "enable" 
debug.log-response-header = "enable" 
debug.log-request-handling = "enable" 
debug.log-file-not-found = "enable" 
debug.log-condition-handling = "enable" 
debug.log-timeouts = "enable" 
```
and reload configuration.
