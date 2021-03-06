grub_write_chain() {
	cat >> $tmpfile <<EOF

# This entry automatically added by the Debian installer for a non-linux OS
# on $partition
title		$title
root		$grubdrive
savedefault
EOF
	# Only set makeactive if grub is installed in the mbr
	if [ "$bootdev" = "(hd0)" ]; then
		cat >> $tmpfile <<EOF
makeactive
EOF
	fi
	# DOS/Windows can't deal with booting from a non-first hard drive
	case $shortname in
	    MS*|Win*)
		grubdisk="$(echo "$grubdrive" | sed 's/^(//; s/)$//; s/,.*//')"
		case $grubdisk in
		    hd0)	;;
		    hd*)
			cat >> $tmpfile <<EOF
map		(hd0) ($grubdisk)
map		($grubdisk) (hd0)
EOF
			;;
		esac
		;;
	esac
	cat >> $tmpfile <<EOF
chainloader	+1

EOF
} # grub_write_chain end

grub2_write_chain() {
	cat >> $tmpfile <<EOF

# This entry automatically added by the Debian installer for a non-linux OS
# on $partition
menuentry "$title" {
	set root=$grubdrive
	chainloader +1
}
EOF

} # grub2_write_chain end

grub_write_linux() {
	cat >> $tmpfile <<EOF

# This entry automatically added by the Debian installer for an existing
# linux installation on $mappedpartition.
title		$label (on $mappedpartition)
root		$grubdrive
kernel		$kernel $params
EOF
	if [ -n "$initrd" ]; then
		cat >> $tmpfile <<EOF
initrd		$initrd
EOF
	fi
	cat >> $tmpfile <<EOF
savedefault
boot

EOF
} # grub_write_linux end

grub2_write_linux() {
	cat >> $tmpfile <<EOF

# This entry automatically added by the Debian installer for an existing
# linux installation on $mappedpartition.
menuentry "$label (on $mappedpartition)" {
	set root=$grubdrive
	linux $kernel $params
EOF
	if [ -n "$initrd" ]; then
		cat >> $tmpfile <<EOF
	initrd $initrd
EOF
	fi
	cat >> $tmpfile <<EOF
}

EOF
} # grub2_write_linux end

grub_write_hurd() {
	cat >> $tmpfile <<EOF

# This entry automatically added by the Debian installer for an existing
# hurd installation on $partition.
title		$title (on $partition)
root		$grubdrive
kernel		/boot/gnumach.gz root=device:$hurddrive
module		/hurd/ext2fs.static --readonly \\
			--multiboot-command-line=\${kernel-command-line} \\
			--host-priv-port=\${host-port} \\
			--device-master-port=\${device-port} \\
			--exec-server-task=\${exec-task} -T typed \${root} \\
			\$(task-create) \$(task-resume)
module		/lib/ld.so.1 /hurd/exec \$(exec-task=task-create)
savedefault
boot

EOF
} # grub_write_hurd end

grub2_write_hurd() {
	cat >> $tmpfile <<EOF

# This entry automatically added by the Debian installer for an existing
# hurd installation on $partition.
menuentry "$title (on $partition)" {
	set root=$grubdrive
	multiboot /boot/gnumach.gz root=device:$hurddrive
	module /hurd/ext2fs.static --readonly \\
			--multiboot-command-line=\${kernel-command-line} \\
			--host-priv-port=\${host-port} \\
			--device-master-port=\${device-port} \\
			--exec-server-task=\${exec-task} -T typed \${root} \\
			\$(task-create) \$(task-resume)
	module /lib/ld.so.1 /hurd/exec \$(exec-task=task-create)
}

EOF
} # grub2_write_hurd end

grub_write_divider() {
	cat >> $ROOT/boot/grub/$menu_file << EOF

# This is a divider, added to separate the menu items below from the Debian
# ones.
title		Other operating systems:
root

EOF
} # grub_write_divider end
