workspace "LiteEngine"
    architecture "x64"
    configurations
    {
        "Debug",
        "Release",
        "Dist"
    }

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

IncludeDir = {}
IncludeDir["GLFW"]  = "LiteEngine/vendor/GLFW/include"
IncludeDir["Glad"]  = "LiteEngine/vendor/Glad/include"
IncludeDir["ImGui"] = "LiteEngine/vendor/imgui"
IncludeDir["glm"]   = "LiteEngine/vendor/glm"
IncludeDir["stb_image"] = "LiteEngine/vendor/stb_image"

include "LiteEngine/vendor/GLFW"
include "LiteEngine/vendor/Glad"
include "LiteEngine/vendor/imgui"


project "LiteEngine"
    location "LiteEngine"
    kind "StaticLib"
    language "C++"
    cppdialect "C++17"
    staticruntime "on"
    buildoptions { "/utf-8" }

    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

    pchheader "hzpch.h"
    pchsource "LiteEngine/src/hzpch.cpp"

    files
    {
        "%{prj.name}/src/**.h",
        "%{prj.name}/src/**.cpp",
        "%{prj.name}/vendor/stb_image/**.h",
        "%{prj.name}/vendor/stb_image/**.cpp",
        "%{prj.name}/vendor/glm/glm/**.hpp",
        "%{prj.name}/vendor/glm/glm/**.inl"
    }

    defines
    {
        "_CRT_SECURE_NO_WARNINGS"
    }

    includedirs
    {
        "%{prj.name}/src",
        "%{prj.name}/vendor/spdlog/include",
        "%{IncludeDir.GLFW}",
        "%{IncludeDir.Glad}",
        "%{IncludeDir.ImGui}",
        "%{IncludeDir.glm}",
        "%{IncludeDir.stb_image}"
    }

    links
    {
        "GLFW",
        "Glad",
        "ImGui",
        "opengl32.lib",
        "dwmapi.lib"
    }

    linkoptions { "/ignore:4006" }

    filter "system:windows"
        systemversion "latest"
        defines
        {
            "LE_PLATFORM_WINDOWS",
            "GLFW_INCLUDE_NONE"
        }

    filter "configurations:Debug"
        defines "LE_DEBUG"
        runtime "Debug"
        symbols "on"

    filter "configurations:Release"
        defines "LE_RELEASE"
        runtime "Release"
        optimize "on"

    filter "configurations:Dist"
        defines "LE_DIST"
        runtime "Release"
        optimize "on"


project "Sandbox"
    location "Sandbox"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++17"
    staticruntime "on"
    buildoptions { "/utf-8" }

    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

    files
    {
        "%{prj.name}/src/**.h",
        "%{prj.name}/src/**.cpp",
    }

    includedirs
    {
        "LiteEngine/vendor/spdlog/include",
        "LiteEngine/src",
        "LiteEngine/vendor",
        "%{IncludeDir.glm}"
    }

    links
    {
        "LiteEngine"
    }

    linkoptions { "/ignore:4006", "/NODEFAULTLIB:LIBCMT" }

    filter "system:windows"
        systemversion "latest"
        defines
        {
            "LE_PLATFORM_WINDOWS"
        }

    filter "configurations:Debug"
        defines "LE_DEBUG"
        runtime "Debug"
        symbols "on"

    filter "configurations:Release"
        defines "LE_RELEASE"
        runtime "Release"
        optimize "on"

    filter "configurations:Dist"
        defines "LE_DIST"
        runtime "Release"
        optimize "on"