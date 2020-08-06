#include "Interface.vxx"
#include "Horizontal.hxx"
#include "Vertical.hxx"

VS_EXTERNAL_API(auto) VapourSynthPluginInit(VSConfigPlugin configFunc, VSRegisterFunction registerFunc, VSPlugin* plugin) {
	VaporGlobals::Identifier = "com.zonked.scanline";
	VaporGlobals::Namespace = "scanline";
	VaporGlobals::Description = "This plugin normalizes the intensity of a scanline with reference to another scanline.";
	VaporInterface::RegisterPlugin(configFunc, plugin);
	VaporInterface::RegisterFilter<Horizontal>(registerFunc, plugin);
	VaporInterface::RegisterFilter<Vertical>(registerFunc, plugin);
}