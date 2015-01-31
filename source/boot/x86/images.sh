#!/bin/sh

get_suffix() {
	if [ "$ASK_TARGET" = "x86_64" ]; then
		echo -n ".elf32"
	fi
	case "$ASK_SIM_FLAGS" in
		*-enable-kvm*)
			echo -n " forcepit"
			;;
	esac
}

create_cd() {
	src="$1"
	dst="$2"
	suffix=`get_suffix`

	cat > $src/boot/grub/menu.lst <<EOF
default 0
timeout 3

title Askatu
kernel /boot/askatu$suffix root=/dev/iso9660-cdrom
module /bin/initloader
module /sbin/pci /dev/pci
module /sbin/ata /sys/devices/ata nodma
module /sbin/iso9660 /dev/iso9660-cdrom cdrom

title Askatu - Test
kernel /boot/askatu_test$suffix
module /bin/initloader
EOF

	genisoimage -U -iso-level 3 -input-charset ascii -R -b boot/grub/stage2_eltorito -no-emul-boot \
		-boot-load-size 4 -boot-info-table -o "$dst" "$src" 1>&2
}

create_mini_cd() {
	src="$1"
	dst="$2"
	suffix=`get_suffix`

	dir=`mktemp -d`
	cp -Rf $src/* $dir
	if [ "$3" = "1" ]; then
		# remove test-files
		rm -Rf $dir/zeros $dir/home/hrniels/* $dir/home/jon/* $dir/root/*
	fi
	# strip all binaries. just ignore the error for others
	find $dir -type f | xargs strip -s 2>/dev/null
	create_cd $dir $dst $suffix
	rm -Rf $dir
}

create_disk() {
	src="$1"
	dst="$2"
	suffix=`get_suffix`

	dir=`mktemp -d`
	cp -R $src/* $dir
	cat > $dir/boot/grub/menu.lst <<EOF
default 0
timeout 3

title Askatu
kernel /boot/askatu$suffix root=/dev/ext2-hda1 swapdev=/dev/hda3
module /bin/initloader
module /sbin/pci /dev/pci
module /sbin/ata /sys/devices/ata
module /sbin/ext2 /dev/ext2-hda1 /dev/hda1

title Askatu - Test
kernel /boot/askatu_test$suffix
module /bin/initloader
EOF

	sudo ./boot/perms.sh $dir
	./tools/disk.py create --part ext2r0 128 "$dir" --part ext2r0 4 - --part nofs 8 - "$dst" 1>&2
	sudo rm -Rf $dir
}
