#pragma once
#include "common/rectangle.h"
#include "types.h"
#include <memory>
#include <tuple>
#include <vector>

// An abstracted RGBA8 texture.
class HostDisplayTexture
{
public:
  virtual ~HostDisplayTexture();

  virtual void* GetHandle() const = 0;
  virtual u32 GetWidth() const = 0;
  virtual u32 GetHeight() const = 0;
};

// Interface to the frontend's renderer.
class HostDisplay
{
public:
  enum class RenderAPI
  {
    None,
    D3D11,
    Vulkan,
    OpenGL,
    OpenGLES
  };

  virtual ~HostDisplay();

  ALWAYS_INLINE s32 GetWindowWidth() const { return m_window_width; }
  ALWAYS_INLINE s32 GetWindowHeight() const { return m_window_height; }

  // Position is relative to the top-left corner of the window.
  ALWAYS_INLINE s32 GetMousePositionX() const { return m_mouse_position_x; }
  ALWAYS_INLINE s32 GetMousePositionY() const { return m_mouse_position_y; }
  ALWAYS_INLINE void SetMousePosition(s32 x, s32 y)
  {
    m_mouse_position_x = x;
    m_mouse_position_y = y;
  }

  virtual RenderAPI GetRenderAPI() const = 0;
  virtual void* GetRenderDevice() const = 0;
  virtual void* GetRenderContext() const = 0;

  /// Call when the window size changes externally to recreate any resources.
  virtual void WindowResized(s32 new_window_width, s32 new_window_height);

  /// Creates an abstracted RGBA8 texture. If dynamic, the texture can be updated with UpdateTexture() below.
  virtual std::unique_ptr<HostDisplayTexture> CreateTexture(u32 width, u32 height, const void* data, u32 data_stride,
                                                            bool dynamic = false) = 0;
  virtual void UpdateTexture(HostDisplayTexture* texture, u32 x, u32 y, u32 width, u32 height, const void* data,
                             u32 data_stride) = 0;

  virtual bool DownloadTexture(const void* texture_handle, u32 x, u32 y, u32 width, u32 height, void* out_data,
                               u32 out_data_stride) = 0;

  virtual void Render() = 0;

  virtual void SetVSync(bool enabled) = 0;

  const s32 GetDisplayTopMargin() const { return m_display_top_margin; }

  void ClearDisplayTexture()
  {
    m_display_texture_handle = nullptr;
    m_display_texture_width = 0;
    m_display_texture_height = 0;
    m_display_texture_view_x = 0;
    m_display_texture_view_y = 0;
    m_display_texture_view_width = 0;
    m_display_texture_view_height = 0;
    m_display_changed = true;
  }

  void SetDisplayTexture(void* texture_handle, s32 texture_width, s32 texture_height, s32 view_x, s32 view_y,
                         s32 view_width, s32 view_height)
  {
    m_display_texture_handle = texture_handle;
    m_display_texture_width = texture_width;
    m_display_texture_height = texture_height;
    m_display_texture_view_x = view_x;
    m_display_texture_view_y = view_y;
    m_display_texture_view_width = view_width;
    m_display_texture_view_height = view_height;
    m_display_changed = true;
  }

  void SetDisplayParameters(s32 display_width, s32 display_height, s32 active_left, s32 active_top, s32 active_width,
                            s32 active_height, float pixel_aspect_ratio)
  {
    m_display_width = display_width;
    m_display_height = display_height;
    m_display_active_left = active_left;
    m_display_active_top = active_top;
    m_display_active_width = active_width;
    m_display_active_height = active_height;
    m_display_pixel_aspect_ratio = pixel_aspect_ratio;
    m_display_changed = true;
  }

  void SetDisplayLinearFiltering(bool enabled) { m_display_linear_filtering = enabled; }
  void SetDisplayTopMargin(s32 height) { m_display_top_margin = height; }
  void SetDisplayIntegerScaling(bool enabled) { m_display_integer_scaling = enabled; }

  /// Sets the software cursor to the specified texture. Ownership of the texture is transferred.
  void SetSoftwareCursor(std::unique_ptr<HostDisplayTexture> texture, float scale = 1.0f);

  /// Sets the software cursor to the specified image.
  bool SetSoftwareCursor(const void* pixels, u32 width, u32 height, u32 stride, float scale = 1.0f);

  /// Sets the software cursor to the specified path (png image).
  bool SetSoftwareCursor(const char* path, float scale = 1.0f);

  /// Disables the software cursor.
  void ClearSoftwareCursor();

  /// Helper function for computing the draw rectangle in a larger window.
  std::tuple<s32, s32, s32, s32> CalculateDrawRect(s32 window_width, s32 window_height, s32 top_margin) const;

  /// Helper function for converting window coordinates to display coordinates.
  std::tuple<s32, s32> ConvertWindowCoordinatesToDisplayCoordinates(s32 window_x, s32 window_y, s32 window_width,
                                                                    s32 window_height, s32 top_margin) const;

  /// Helper function to save texture data to a PNG. If flip_y is set, the image will be flipped aka OpenGL.
  bool WriteTextureToFile(const void* texture_handle, u32 x, u32 y, u32 width, u32 height, const char* filename,
                          bool clear_alpha = true, bool flip_y = false, u32 resize_width = 0, u32 resize_height = 0);

  /// Helper function to save current display texture to PNG.
  bool WriteDisplayTextureToFile(const char* filename, bool full_resolution = true, bool apply_aspect_ratio = true);

  /// Helper function to save current display texture to a buffer.
  bool WriteDisplayTextureToBuffer(std::vector<u32>* buffer, u32 resize_width = 0, u32 resize_height = 0,
                                   bool clear_alpha = true);

protected:
  ALWAYS_INLINE bool HasSoftwareCursor() const { return static_cast<bool>(m_cursor_texture); }
  ALWAYS_INLINE bool HasDisplayTexture() const { return (m_display_texture_handle != nullptr); }

  void CalculateDrawRect(s32 window_width, s32 window_height, s32* out_left, s32* out_top, s32* out_width,
                         s32* out_height, s32* out_left_padding, s32* out_top_padding, float* out_scale,
                         float* out_y_scale) const;

  std::tuple<s32, s32, s32, s32> CalculateSoftwareCursorDrawRect() const;

  s32 m_window_width = 0;
  s32 m_window_height = 0;

  s32 m_mouse_position_x = 0;
  s32 m_mouse_position_y = 0;

  s32 m_display_width = 0;
  s32 m_display_height = 0;
  s32 m_display_active_left = 0;
  s32 m_display_active_top = 0;
  s32 m_display_active_width = 0;
  s32 m_display_active_height = 0;
  float m_display_pixel_aspect_ratio = 1.0f;

  void* m_display_texture_handle = nullptr;
  s32 m_display_texture_width = 0;
  s32 m_display_texture_height = 0;
  s32 m_display_texture_view_x = 0;
  s32 m_display_texture_view_y = 0;
  s32 m_display_texture_view_width = 0;
  s32 m_display_texture_view_height = 0;

  s32 m_display_top_margin = 0;

  std::unique_ptr<HostDisplayTexture> m_cursor_texture;
  float m_cursor_texture_scale = 1.0f;

  bool m_display_linear_filtering = false;
  bool m_display_changed = false;
  bool m_display_integer_scaling = false;
};
