// Copyright 2008 Dolphin Emulator Project
// Licensed under GPLv2+
// Refer to the license.txt file included.

#pragma once

#include <memory>

#include "VideoBackends/D3D12/D3DTexture.h"
#include "VideoCommon/TextureCacheBase.h"

namespace DX12
{
class D3DStreamBuffer;

class TextureCache final : public TextureCacheBase
{
public:
  TextureCache();
  ~TextureCache();

  virtual void BindTextures();

private:
  struct TCacheEntry : TCacheEntryBase
  {
    D3DTexture2D* const m_texture = nullptr;
    D3D12_CPU_DESCRIPTOR_HANDLE m_texture_srv_cpu_handle = {};
    D3D12_GPU_DESCRIPTOR_HANDLE m_texture_srv_gpu_handle = {};
    D3D12_CPU_DESCRIPTOR_HANDLE m_texture_srv_gpu_handle_cpu_shadow = {};

    TCacheEntry(const TCacheEntryConfig& config, D3DTexture2D* tex)
        : TCacheEntryBase(config), m_texture(tex)
    {
    }
    ~TCacheEntry();

    void CopyRectangleFromTexture(const TCacheEntryBase* source,
                                  const MathUtil::Rectangle<int>& src_rect,
                                  const MathUtil::Rectangle<int>& dst_rect) override;

    void Load(const u8* buffer, u32 width, u32 height, u32 expanded_width, u32 levels) override;

    void FromRenderTarget(bool is_depth_copy, const EFBRectangle& src_rect, bool scale_by_half,
                          unsigned int cbuf_id, const float* colmat) override;

    void Bind(unsigned int stage) override;
    bool Save(const std::string& filename, unsigned int level) override;
  };

  TCacheEntryBase* CreateTexture(const TCacheEntryConfig& config) override;

  u64 EncodeToRamFromTexture(u32 address, void* source_texture, u32 source_width, u32 source_height,
                             bool is_from_z_buffer, bool is_intensity_format, u32 copy_format,
                             int scale_by_half, const EFBRectangle& source)
  {
    return 0;
  };

  void ConvertTexture(TCacheEntryBase* entry, TCacheEntryBase* unconverted, void* palette,
                      TlutFormat format) override;

  void CopyEFB(u8* dst, const EFBCopyFormat& format, u32 native_width, u32 bytes_per_row,
               u32 num_blocks_y, u32 memory_stride, bool is_depth_copy,
               const EFBRectangle& src_rect, bool scale_by_half) override;

  bool CompileShaders() override { return true; }
  void DeleteShaders() override {}
  std::unique_ptr<D3DStreamBuffer> m_palette_stream_buffer;

  ID3D12Resource* m_palette_uniform_buffer = nullptr;
  D3D12_SHADER_BYTECODE m_palette_pixel_shaders[3] = {};
};
}
