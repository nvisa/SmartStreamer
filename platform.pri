PLATFORM_TEGRA = $$system("head -n 1 /etc/nv_tegra_release | awk '{print $9}' | tr -d ,")
message($$PLATFORM_TEGRA)

ardbeg {
# jetpack tegra tk1 What is apalis tk1 board name ??? !!
    CONFIG += tk1
} else:t210ref {
# TX1
    CONFIG += tx1
} else:t186ref {
# TX2
    CONFIG += tx2
}
