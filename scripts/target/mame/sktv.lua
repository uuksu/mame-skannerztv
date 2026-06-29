-- license:BSD-3-Clause
-- copyright-holders:MAMEdev Team

---------------------------------------------------------------------------
--
--   sktv.lua
--
--   Minimal build for Skannerz TV (rad_sktv) only.
--   Build with: make SUBTARGET=sktv asmjs
--
---------------------------------------------------------------------------

CPUS["UNSP"] = true

MACHINES["SPG2XX"] = true
MACHINES["WATCHDOG"] = true
MACHINES["I2CMEM"] = true
MACHINES["EEPROMDEV"] = true

SOUNDS["IMAADPCM"] = true

BUSES["GENERIC"] = true

function createProjects_mame_sktv(_target, _subtarget)
	project ("mame_sktv")
	targetsubdir(_target .."_" .. _subtarget)
	kind (LIBTYPE)
	uuid (os.uuid("drv-mame-sktv"))
	addprojectflags()
	precompiledheaders_novs()

	includedirs {
		MAME_DIR .. "src/osd",
		MAME_DIR .. "src/emu",
		MAME_DIR .. "src/devices",
		MAME_DIR .. "src/mame/shared",
		MAME_DIR .. "src/lib",
		MAME_DIR .. "src/lib/util",
		MAME_DIR .. "3rdparty",
		GEN_DIR  .. "mame/layout",
	}

	files {
		MAME_DIR .. "src/mame/tvgames/spg2xx.cpp",
		MAME_DIR .. "src/mame/tvgames/spg2xx.h",
		MAME_DIR .. "src/mame/tvgames/spg2xx_skannerztv.cpp",
	}
end

function linkProjects_mame_sktv(_target, _subtarget)
	links {
		"mame_sktv",
	}
end
