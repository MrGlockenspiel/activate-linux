set_project("activate-linux")

set_languages("gnu99")
add_cflags("-Os", "-Wall", "-Wextra", "-Wpedantic")

option("X11")
if not is_plat("windows") then
	set_default(true)
else
	set_default(false)
end
set_showmenu(true)
set_description("X11 Backend")
option_end()

option("Wayland")
if not is_plat("windows") then
	set_default(true)
else
	set_default(false)
end
set_showmenu(true)
set_description("Wayland backend")
option_end()

option("GDI")
if is_plat("windows") then
	set_default(true)
else
	set_default(false)
end
set_showmenu(true)
set_description("GDI Backed (win32)")
option_end()

option("libconfig")
set_default(true)
set_showmenu(true)
set_description("libconfig support")
option_end()

if has_config("X11") then
	add_requires("cairo", "x11", "xfixes", "xinerama", "xrandr", "xext")
	add_syslinks("cairo", "X11", "Xfixes", "Xinerama", "Xrandr", "Xext")
	add_defines("COLOR_HELP", "CAIRO", "X11")
	add_files("src/x11/*.cgen", { rule = "cgen" })
	add_files("src/x11/*.c")
	add_files("src/x11/XEventTypes.c", { always_added = true })
end

if has_config("Wayland") then
	add_requires("cairo", "wayland-client")
	add_syslinks("cairo", "wayland-client", "rt")
	add_defines("COLOR_HELP", "CAIRO", "WAYLAND")
	add_files("src/wayland/*.hgen", { rule = "hgen" })
	add_files("src/wayland/*.cgen", { rule = "cgen" })
	add_files("src/wayland/*.c")
	add_files("src/wayland/wlr-layer-shell-unstable-v1.c", { always_added = true })
	add_files("src/wayland/xdg-shell.c", { always_added = true })
end

if has_config("GDI") then
	if is_arch("i386") then
		add_cflags("-m32")
	end
	add_syslinks("gdi32")
	add_defines("GDI")
	add_files("src/gdi/*.c")
end

if has_config("libconfig") then
	add_syslinks("config")
end

rule("hgen")
set_extensions(".hgen")
after_load(function(target)
	local rule = target:rule("c.build"):clone()
	rule:add("deps", "hgen", { order = true })
	target:rule_add(rule)
end)
on_build_file(function(target, sourcefile, opt)
	import("utils.progress")
	local targetfile = string.gsub(sourcefile, ".hgen", ".h")
	local cmd = "sh -- " .. sourcefile .. " " .. targetfile
	os.vrunv(cmd)
	progress.show(opt.progress, "${color.build.object} GEN\t%s", sourcefile)
end)
rule_end()

rule("cgen")
set_extensions(".cgen")
after_load(function(target)
	local rule = target:rule("c.build"):clone()
	rule:add("deps", "cgen", { order = true })
	target:rule_add(rule)
end)
on_build_file(function(target, sourcefile, opt)
	import("utils.progress")
	local targetfile = string.gsub(sourcefile, ".cgen", ".c")
	local cmd = "sh -- " .. sourcefile .. " " .. targetfile
	os.vrunv(cmd)
	progress.show(opt.progress, "${color.build.object} GEN\t%s", sourcefile)
end)
rule_end()

target("activate-linux")
set_kind("binary")
add_rules("cgen", "hgen")
add_files("src/*.c", { always_added = true })
target_end()
