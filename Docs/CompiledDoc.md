# BQ24295 Zephyr Driver - Development Commands

## Build the application

```bash
west build -p always -b esp32s3_devkitc/esp32s3/procpu .
```

---

## Flash the application

```bash
west flash
```

---

## Run Twister build test

```bash
./scripts/twister -T tests/drivers/build_all/charger
```

---

## Find unused variable warnings from the Twister build log

```bash
grep -n "unused variable" \
/home/sacrasystems/zephyrproject/zephyr/twister-out/native_sim_native/host_gnu/tests/drivers/build_all/charger/drivers.charger.build/build.log
```

---

## Format the source code

```bash
clang-format -i drivers/charger/charger_bq24295.c
```

---

## Perform code style review

```bash
./scripts/checkpatch.pl --file drivers/charger/charger_bq24295.c
```

---

# Pull Request Guidelines

## Contribution Workflow

Navigate to the Zephyr repository:

```bash
cd zephyrproject/zephyr
```

### 1. Fork the Zephyr repository

Create a fork of the Zephyr repository under your personal GitHub account.

---

### 2. Rename the upstream remote

Rename the existing `origin` remote to `upstream`:

```bash
git remote rename origin upstream
```

---

### 3. Add your fork as the new origin

Replace `<your-github-id>` with your GitHub username.

```bash
git remote add origin https://github.com/<your-github-id>/zephyr
```

Verify the configured remotes:

```bash
git remote -v
```

Expected output:

```text
origin   https://github.com/<your-github-id>/zephyr (fetch)
origin   https://github.com/<your-github-id>/zephyr (push)
upstream https://github.com/zephyrproject-rtos/zephyr (fetch)
upstream https://github.com/zephyrproject-rtos/zephyr (push)
```

---

### 4. Create a topic branch

Create a new branch from `main`:

```bash
git switch main
git switch -c fix_comment_typo
```

Some Zephyr subsystems use different development branches. In that case:

```bash
git switch -c fix_out_of_date_patch origin/net
```

---

### 5. Develop and test

Make your changes and test them thoroughly.

Build:

```bash
west build -p always -b esp32s3_devkitc/esp32s3/procpu .
```

Run Twister:

```bash
./scripts/twister -T tests/drivers/build_all/charger
```

---

### 6. Stage your changes

Stage the modified files:

```bash
git add <modified-files>
```

or interactively:

```bash
git add -p
```

Check the repository status:

```bash
git status
```

Review the staged changes:

```bash
git diff --cached
```

---

### 7. Commit your changes

Create a signed commit:

```bash
git commit -s
```

The `-s` option automatically adds the required `Signed-off-by` line for the Developer Certificate of Origin (DCO).

---

### 8. Push your branch

```bash
git push origin fix_comment_typo
```

---

### 9. Open a Pull Request

1. Open your fork on GitHub.
2. Click **Compare & pull request**.
3. Verify the target branch is `main`.
4. Review the title and description.
5. Submit the pull request.

Reviewers will be suggested automatically based on the `MAINTAINERS` file.

Monitor your pull requests at:

<https://github.com/zephyrproject-rtos/zephyr/pulls>

---

### 10. Work on another issue

While waiting for reviews, create a new branch from `main`:

```bash
git switch main
git switch -c fix_another_issue
```

---

### 11. Address review comments

Start an interactive rebase:

```bash
git rebase -i <offending-commit-id>^
```

Replace `pick` with `edit` for the commit you want to modify.

After making changes:

```bash
git add <files>
git rebase --continue
```

Update the pull request:

```bash
git push --force origin fix_comment_typo
```

---

### 12. Resolve merge conflicts

Fetch the latest changes:

```bash
git fetch --all
```

Rebase onto the latest upstream branch:

```bash
git rebase --ignore-whitespace upstream/main
```

Resolve any merge conflicts, then update the pull request:

```bash
git push --force origin fix_comment_typo
```

---

## Notes

- Keep pull requests small and focused.
- Test changes thoroughly before submitting.
- Update documentation when applicable.
- After rebasing, use `git push --force` to update the existing pull request.
- If CI fails, fix the issues, amend the commits through rebase, and force-push again.

## Reference

Official Zephyr contribution guide:

https://docs.zephyrproject.org/latest/contribute/guidelines.html#contribution-workflow