# gam541-nutmeg
Team repository for DigiPen GAM541 team codenamed nutmeg

# Git Workflow Guide for Students

This guide walks you through the full process of working with a GitHub repository: cloning it, creating your own branch, making changes, pushing your work, and creating a Pull Request (PR).  
It is written for students who have only basic familiarity with Git and GitHub.

---

## 1. Cloning the Repository to Your Local Machine

Before you start, make sure you have:

- Git installed on your computer  
- A GitHub account  
- Access to the repository you need to work on

**Steps:**

1. Go to the repository page on GitHub in your web browser.
2. Click the green **Code** button.
3. Copy the HTTPS URL (it looks like `https://github.com/organization/repository.git`).
4. Open your terminal (Command Prompt, PowerShell, or macOS Terminal).
5. Navigate to the folder where you want the project to live.
6. Run the following command:

```
git clone <repository-url>
```

Example:

```
git clone https://github.com/example-org/example-repo.git
```

7. Move into the project folder:

```
cd example-repo
```

---

## 2. Creating a Local Branch From `main`

You will create a branch based on the `main` branch.  
Your branch name should follow this pattern:

```
firstname/featureNameOrBugId
```

Examples:

- `alex/add-login-button`
- `maria/fix-142-broken-link`

**Steps:**

1. Make sure you are on the `main` branch:

```
git checkout main
```

2. Pull the latest changes so your copy is up to date:

```
git pull
```

3. Create your new branch:

```
git checkout -b firstname/featureNameOrBugId
```

This creates the branch and switches you to it.

---

## 3. Making Changes on Your Local Branch

Now you can open the project in your editor (VS Code, IntelliJ, etc.) and make your changes.

When you are ready to save your work:

1. Check which files you changed:

```
git status
```

2. Add the files you want to include in your commit:

```
git add <filename>
```

Or add everything:

```
git add .
```

3. Commit your changes with a meaningful message:

```
git commit -m "Describe what you changed"
```

You can repeat this process as many times as needed while you work.

---

## 4. Pushing Your Branch to GitHub

When you are done testing your changes locally:

1. Push your branch to the remote repository:

```
git push -u origin firstname/featureNameOrBugId
```

The `-u` flag sets up tracking so future pushes can be done with just `git push`.

---

## 5. Creating a Pull Request (PR)

Once your branch is pushed:

1. Go to the repository on GitHub in your browser.
2. You should see a message offering to create a Pull Request from your branch.  
   If not, click the **Pull Requests** tab and then **New Pull Request**.
3. Make sure the base branch is `main` and the compare branch is your branch.
4. Add a clear title and description explaining what you changed.
5. Submit the Pull Request.

---

## 6. Waiting for Code Review

A teammate will now review your code.  
They may:

- Approve it  
- Request changes  
- Leave comments or questions  

If changes are requested:

1. Make the updates on your local branch.
2. Commit them.
3. Push again using:

```
git push
```

Your PR will automatically update.

---

## 7. Merging Into `main`

Once your PR is approved:

1. Go to the Pull Request page on GitHub.
2. Click the **Merge** button (usually labeled “Merge pull request”).
3. Confirm the merge.
4. After merging, you can delete your branch on GitHub if you want (GitHub will offer a button for this).

Finally, update your local `main` branch:

```
git checkout main
git pull
```

---

## Summary of Common Commands

```
git clone <url>
git checkout main
git pull
git checkout -b firstname/featureNameOrBugId
git status
git add .
git commit -m "message"
git push -u origin firstname/featureNameOrBugId
```

