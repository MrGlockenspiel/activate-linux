#!/usr/bin/env bash
set -xeou

###
# Run it on fedora or else
###

rpm_dir="$HOME/rpmbuild"
tar_dir="/tmp"
cur_dir=$(pwd)

tar_suffix=".tar.gz"
tar_name=activate-linux-$(sed -nr 's/^Version:\s+(.+$)/\1/p' activate-linux.spec)
tar_file=$tar_name$tar_suffix

# Create rpmbuild structure
mkdir -p "$rpm_dir/RPMS"
mkdir -p "$rpm_dir/SPECS"
mkdir -p "$rpm_dir/SOURCES"

# Build tarfile
rm -rf "$tar_dir/tarbuild/"
mkdir -p "$tar_dir/tarbuild/"
cp -r . "$tar_dir/tarbuild/$tar_name"
cd "$tar_dir/tarbuild" || exit
tar -czf $tar_dir/$tar_file .
cd "$cur_dir" || exit

# Build RPM
cp $tar_dir/$tar_file "$rpm_dir/SOURCES/$tar_file"
cp activate-linux.spec "$rpm_dir/SPECS/"
rpmbuild -bb "$rpm_dir/SPECS/activate-linux.spec" --nodebuginfo

# Clean
rm -rf $tar_dir/tarbuild
rm "$tar_dir/$tar_file"
