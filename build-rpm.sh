#!/usr/bin/env bash

###
# Run it on fedora or else
###

# Create rpmbuild structure
rpm_dir="$HOME/rpmbuild"
mkdir -p "$rpm_dir/RPMS"
mkdir -p "$rpm_dir/SPECS"
mkdir -p "$rpm_dir/SOURCES"

# Build tarfile
tar_out="/tmp/activate-linux-git.tar.gz"
cur_dir=$(pwd)
mkdir -p /tmp/tarbuild/
cp -r . /tmp/tarbuild/activate-linux-git
cd /tmp/tarbuild || exit
tar -czf $tar_out .
cd "$cur_dir" || exit

# Build RPM
cp $tar_out "$rpm_dir/SOURCES/"
cp activate-linux-rpm.spec "$rpm_dir/SPECS/"
rpmbuild -bb "$rpm_dir/SPECS/activate-linux-rpm.spec" --nodebuginfo

# Clean
rm -rf /tmp/tarbuild
rm $tar_out
