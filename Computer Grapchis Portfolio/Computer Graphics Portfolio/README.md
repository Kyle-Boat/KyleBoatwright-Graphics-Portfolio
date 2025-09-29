# CS 343 project repository
Welcome to CS 343!  This repository contains instructions and starter templates for all of the projects in this course.

After several years of difficulties reconciling openFrameworks and source control, I finally went ahead and just created this repository, which contains its own installation of openFrameworks (you don't need to download openFrameworks separately) as well as the starter files and instructions for each major project.

I'm opting to standardize the use of GitLab rather than GitHub in this course for several reasons:
* As a computer graphics course, there are some large files in this repo (i.e. images, 3D models, heighmaps) that require Large File Storage (LFS) support.  At the time of writing, GitLab has higher LFS storage limits for free repositories than GitHub.
* GitLab supports private forks (see below), which makes it easier to create your own private copy of the repo to commit to, which your classmates can't copy.
* There are ongoing concerns about the privacy of code on GitHub, particularly with regards to code being used to train GitHub Copilot.  Because of this, I don't want to require anyone to use GitHub.

To ensure the privacy and academic integrity of your work for this course, please make a ***PRIVATE*** fork of this repository on GitLab to keep your own work hidden from classmates who are not working with you.

At the end of the semester, if you want to upload one or more projects to a public repo for your portfolio, feel please to do so, but make sure you do it in a new repo that is ***not*** linked as a fork of this one, so that there isn't an obvious link between this repository and solutions to the projects.  (You may need to manually enable LFS on that new repo, which I would be happy to help you set up if you decide to do this.)

I recommend using TortoiseGit (https://tortoisegit.org/) to initially clone your private fork of the repo and to handle more complex source control issues (TortoiseGit also includes Git command line, if necessary).  For day-to-day commits, pushes, and pulls, the integrated Git support in Visual Studio should be sufficient.

You can find starter templates and instructions for each of the projects for this course within the **apps** folder.  You will submit each project by committing and pushing to GitHub, and submitting the URL of your repository to Canvas.

Throughout the semester, I may commit and push lecture examples to a branch of this repository.  You can access these branches in your own forked repository by running the batch script "addUpstreamBranches.bat" that is found in the root directory of this repository (double-click to run should work fine).  After doing this, you can find these branches in Visual Studio by navigating to Git > Manage Branches and opening remotes > upstream in the "Branches" tree view.
