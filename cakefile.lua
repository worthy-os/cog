--------------------------------------------------------------------------------
-- Define every project on the mac platform.
--
-- Declaration types:
--    framework
--    shared
--    static
--------------------------------------------------------------------------------

if platform:is'apple'then
  print'Constructing eon workspace for Xcode'
end

--------------------------------------------------------------------------------
-- Define every project on the 64-bit windows platform.
--------------------------------------------------------------------------------

if platform.is'win64'then
  print'Constructing eon workspace for Visual Studio'
end

--------------------------------------------------------------------------------
-- Build the project cross platform and save.
--------------------------------------------------------------------------------

local eon = workspace:declare'eon'--> Will create eon.xcworkspace
local lib = eon:declare'eon'--> Will create eon.xcproj
lib : include'usr/engine/include'
    : sources'usr/engine/src'
    : target'framework'
if platform.save( eon, 'tmp' )then
  return true
end
error"Couldn't save to platform."
return false
