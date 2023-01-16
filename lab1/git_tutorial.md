# Git tutorial

## Git overview

git is a <b>distributed version control</b> system. In lab setup this means that you have two identical repositories, one local repository in your development VM and other remote repository at TUAS gitlab service. Both repositories contain full version history (when properly synced), so the setup acts as backup as well.

A version history consists of sequence of <b>commits</b>. Each commit is a set of changes (possibly to multiple files) together with commit message. A good commit  
- Does not break the code: After each commit the code compiles (although it might not work properly, but at least there are no errors). So you should compile (and test) your code before the commit.
- Has reasonable workhour limit. If your code does not compile (yet), you might want to backup your progress daily.
- Has reasonable size. It is good to have multiple smaller commits, so if you add a major feature to code, you might first define necessary data structures and create empty functions with necessary signature (parameter and return types), and maybe bind those functions to main logic (add function calls). This should compile OK and could be committed. Next commit could add features inside the added functions.  

<b>Branch</b> is a parallel sequence of commits. Branches can start at any commit in history (having done multiple small commits during development helps to find good branch points later if necessary). Branches can be <b>merged</b>, i.e. the changes are synced so that two merged branches are in same state. Branches can be used for multiple reasons
- <b>Feature branches</b> are used for adding major features. Developer creates a new branch from current head, adds all commits into that bracnh until code is final and tested. Then feature branch is merged into main, and that feature branch is deleted. Feature branches scale well: it is possible for multiple developers to work on same project, with each developer team on a separate parallel feature branch.
- <b>Release branches</b> are used for controlling the release of features to customers. For example the "development" branch is the work-in-progress where feature branches are started and merged. "Release" branch is merged from stable development points. In large teams it might be necessary to have a separate "testing" branch between those two.
- You should start a new temporary branch when you start tinkering with the code with multiple commits: if you do not know how to proceed, so you might need to edit/delete earlier working code to see is there another way of getting the new feature done. You know that main branch still has the other development idea in working state.

## Lab assignment 1

Study the videos below and answer to questions into file `embedded-linux-labs/lab1/git-study.md`  
In this case it makes sense to create the file and write the answers using TUAS gitlab web interface to your personal repository. 

https://git-scm.com/video/what-is-version-control (5:58)  
Q1: What are the benefits of using git version control?

https://git-scm.com/video/what-is-git (8:15)  
Q2: What is a distributed version control system?

https://git-scm.com/video/get-going (4:26)  
Q3: What are the most important git commands in terminal use?


## GitLab overview

GitLab (like GitHub) is a <b>DevOps lifecycle tool</b>. It adds software product management tools on top of git version control. The main features are
- <b>Project</b> binds to a git repository adding management tools
- <b>Issues</b> (tickets) you can use to plan development and report problems. Issues are bound to workflow; issue triggers development (commits, testing), reviews, approval process, and build automation & deployment.
- <b>CI pipeline</b> automates build, security scan, testing, deployment etc operations. It is possible to define GitLab Runner server that spins off a container that handles the detail work. For example, when committing a tag to a release could trigger CI pipeline that builds the code and deploys it into target.

## Lab assignment 2

Study the videos below and append your answers into file `embedded-linux-labs/lab1/git-study.md`  

[Introduction to GitLab](https://youtu.be/_4SmIyQ5eis?t=90) (59:51)  
Q4: Explain "GitLab Recommended Process"  
Q5: What is a "Merge Request"?

For other material, see   
https://docs.gitlab.com/ee/tutorials/  


## General guidelines for lab work

You have two user interfaces to your repository:
- In your local repository you can edit files (using command-line nano or GUI vscode), commit and push. This is the environment where you develop; any changes you make, you can test them before commit.
- In TUAS gitlab web interface you can also edit any files. However it does not make sense to make code changes here - this would lead to commits not compiled nor tested. On the other hand this would the perfect place to write documentation (markdown files) because the web interface shows previews from those as well (and gitlab markdown has some extensions that don't render that well in vscode etc).  

**What happens when the teacher changes the lab instructions in the git, but you have your own forked repository?** Yes, you will not get the updates. To solve this, you need to add another remote repository to your project: https://digitaldrummerj.me/git-sync-fork-to-master/  
Just change the https-formed addresses to git@-style addresses and replace "master" with "main"


In general,
- commit often
- learn to use feature branches for development

