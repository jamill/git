#!/bin/sh
#
#

test_description='git status collapse ignored'

. ./test-lib.sh


cat >.gitignore <<\EOF
*.ign
ignored_dir/
EOF

test_expect_success 'setup' '
	mkdir tracked &&
	mkdir tracked_ignored &&
	mkdir ignored &&
	mkdir untracked &&
	mkdir untracked_ignored &&
	mkdir ignored_dir &&
	mkdir dir_1 &&
	mkdir dir_1/ignored_dir &&
	mkdir dir_1/dir_1_1 &&
	mkdir dir_1/dir_1_1/tracked_ignored &&
	mkdir dir_1/dir_1_1/ignored &&
	mkdir dir_1/dir_1_1/untracked &&
	mkdir dir_1/dir_1_1/untracked_ignored &&
	mkdir dir_1/dir_1_1/ignored_dir &&
	mkdir dir_1/dir_1_1/ignored_with_sub_untracked &&
	mkdir dir_1/dir_1_1/ignored_with_sub_untracked/untracked &&
	mkdir dir_1/dir_1_1/untracked_with_sub_ignored &&
	mkdir dir_1/dir_1_1/untracked_with_sub_ignored/ignored &&
	echo 1 >tracked/tracked.txt &&
	echo 1 >ignored/ignored_1.ign &&
	echo 1 >ignored/ignored_2.ign &&
	echo 1 >tracked_ignored/tracked_1.txt &&
	echo 1 >tracked_ignored/tracked_2.txt &&
	echo 1 >tracked_ignored/ignored_1.ign &&
	echo 1 >tracked_ignored/ignored_2.ign &&
	echo 1 >untracked_ignored/ignored_1.ign &&
	echo 1 >untracked_ignored/ignored_2.ign &&
	echo 1 >ignored_dir/test.txt &&

	echo 1 >dir_1/dir_1_1/ignored/ignored_1.ign &&
	echo 1 >dir_1/dir_1_1/tracked_ignored/tracked_1.txt &&
	echo 1 >dir_1/dir_1_1/tracked_ignored/tracked_2.txt &&
	echo 1 >dir_1/dir_1_1/tracked_ignored/ignored_1.ign &&
	echo 1 >dir_1/dir_1_1/tracked_ignored/ignored_2.ign &&
	echo 1 >dir_1/dir_1_1/untracked_ignored/ignored_1.ign &&
	echo 1 >dir_1/dir_1_1/untracked_ignored/ignored_2.ign &&
	echo 1 >dir_1/dir_1_1/ignored_dir/test.txt &&
	echo 1 >dir_1/dir_1_1/ignored_dir/test2.txt &&

	echo 1 >dir_1/dir_1_1/ignored_with_sub_untracked/ignored_1.ign &&
	echo 1 >dir_1/dir_1_1/ignored_with_sub_untracked/ignored_2.ign &&

	echo 1 >dir_1/dir_1_1/untracked_with_sub_ignored/ignored/ignored_1.ign &&
	echo 1 >dir_1/dir_1_1/untracked_with_sub_ignored/ignored/ignored_2.ign &&

	git add . &&

	test_tick &&

	git commit -m initial &&
	echo 1 >untracked_ignored/untracked_1.txt &&
	echo 1 >untracked_ignored/untracked_2.txt &&
	echo 1 >untracked/untracked_1.txt &&
	echo 1 >dir_1/dir_1_1/untracked/untracked_1.txt &&
	echo 1 >dir_1/dir_1_1/untracked_ignored/untracked_1.txt &&
	echo 1 >dir_1/dir_1_1/untracked_with_sub_ignored/untracked_1.txt
'

cat >expect <<\EOF
On branch master
Untracked files:
  (use "git add <file>..." to include in what will be committed)

	dir_1/dir_1_1/untracked/untracked_1.txt
	dir_1/dir_1_1/untracked_ignored/untracked_1.txt
	dir_1/dir_1_1/untracked_with_sub_ignored/untracked_1.txt
	expect
	output
	untracked/untracked_1.txt
	untracked_ignored/untracked_1.txt
	untracked_ignored/untracked_2.txt

Ignored files:
  (use "git add -f <file>..." to include in what will be committed)

	dir_1/dir_1_1/ignored/ignored_1.ign
	dir_1/dir_1_1/ignored_dir/
	dir_1/dir_1_1/ignored_with_sub_untracked/ignored_1.ign
	dir_1/dir_1_1/ignored_with_sub_untracked/ignored_2.ign
	dir_1/dir_1_1/tracked_ignored/ignored_1.ign
	dir_1/dir_1_1/tracked_ignored/ignored_2.ign
	dir_1/dir_1_1/untracked_ignored/ignored_1.ign
	dir_1/dir_1_1/untracked_ignored/ignored_2.ign
	dir_1/dir_1_1/untracked_with_sub_ignored/ignored/ignored_1.ign
	dir_1/dir_1_1/untracked_with_sub_ignored/ignored/ignored_2.ign
	ignored/ignored_1.ign
	ignored/ignored_2.ign
	ignored_dir/
	tracked_ignored/ignored_1.ign
	tracked_ignored/ignored_2.ign
	untracked_ignored/ignored_1.ign
	untracked_ignored/ignored_2.ign

nothing added to commit but untracked files present (use "git add" to track)
EOF

test_expect_success 'status --ignore --untracked-files=all --show-ignored-directory' '
	git status --ignored --untracked-files=all --show-ignored-directory >output &&
	test_i18ncmp expect output
'

test_done
