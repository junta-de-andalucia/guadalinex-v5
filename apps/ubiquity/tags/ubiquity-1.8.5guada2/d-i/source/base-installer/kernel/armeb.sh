arch_get_kernel_flavour () {
	case "$SUBARCH" in
		ixp4xx)
			echo "$SUBARCH"
			return 0
		;;
		*)
			warning "Unknown $ARCH subarchitecture '$SUBARCH'."
			return 1
		;;
	esac
}

arch_check_usable_kernel () {
	# Subarchitecture must match exactly.
	if expr "$1" : ".*-$2\$" >/dev/null; then return 0; fi
	return 1
}

arch_get_kernel () {
	echo "linux-image-$KERNEL_MAJOR-$1"
}
