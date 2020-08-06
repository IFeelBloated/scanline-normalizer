#pragma once
#include "Interface.vxx"

struct Horizontal final {
	static constexpr auto Name = "Horizontal";
	static constexpr auto Parameters = "clip:clip;idx:int;ref_idx:int";
	self(InputClip, Clip{});
	self(ScanlineIndex, 0);
	self(ReferenceIndex, 0);
	auto Initialize(auto Arguments, auto Console) {
		InputClip = Arguments["clip"];
		ScanlineIndex = Arguments["idx"];
		ReferenceIndex = Arguments["ref_idx"];
		if (!InputClip.WithConstantFormat() || !InputClip.WithConstantDimensions() || !InputClip.Is444() || !InputClip.IsSinglePrecision())
			return Console.RaiseError("input must be of GrayS, YUV444PS or RGBS format.");
		if (ScanlineIndex < 0 || ScanlineIndex >= InputClip.Height)
			return Console.RaiseError("idx is going out of bounds.");
		if (ReferenceIndex < 0 || ReferenceIndex >= InputClip.Height)
			return Console.RaiseError("ref_idx is going out of bounds.");
		return true;
	}
	auto RegisterVideoInfo(auto Core) {
		return InputClip.ExposeVideoInfo();
	}
	auto RequestReferenceFrames(auto Index, auto FrameContext) {
		InputClip.RequestFrame(Index, FrameContext);
	}
	auto DrawFrame(auto Index, auto Core, auto FrameContext) {
		auto InputFrame = InputClip.GetFrame<const float, true>(Index, FrameContext);
		auto ProcessedFrame = Core.CopyFrame(InputFrame);
		auto CalculateStatistics = [&](auto Channel) {
			auto Intensity = 0.;
			auto TargetIntensity = 0.;
			for (auto x : Range{ InputClip.Width }) {
				Intensity += std::abs(InputFrame[Channel][ScanlineIndex][x]);
				TargetIntensity += std::abs(InputFrame[Channel][ReferenceIndex][x]);
			}
			return std::array{ Intensity, TargetIntensity };
		};
		auto NormalizeScanline = [&](auto Channel, auto&& Statistics) {
			auto [Intensity, TargetIntensity] = Statistics;
			for (auto x : Range{ InputClip.Width })
				ProcessedFrame[Channel][ScanlineIndex][x] = ProcessedFrame[Channel][ScanlineIndex][x] / Intensity * TargetIntensity;
		};
		for (auto c : Range{ InputFrame.PlaneCount })
			NormalizeScanline(c, CalculateStatistics(c));
		return ProcessedFrame.Leak();
	}
};