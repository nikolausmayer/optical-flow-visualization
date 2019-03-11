/**
 * Author: Nikolaus Mayer, 2017-2019 (mayern@cs.uni-freiburg.de)
 */

#include <cmath>
#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <random>
#include <sstream>
#include <string>
/// AGG (Anti-Grain Geometry)
#include <agg_ellipse.h>
#include <agg_pixfmt_gray.h>
#include <agg_pixfmt_rgb.h>
#include <agg_pixfmt_rgba.h>
#include <agg_renderer_base.h>
#include <agg_renderer_scanline.h>
#include <agg_scanline_u.h>
#include <agg_rasterizer_scanline_aa.h>
#include <agg_path_storage.h>
#include <agg_conv_curve.h>
#include <agg_conv_dash.h>
#include <agg_conv_stroke.h>
#include <agg_conv_transform.h>
#include <agg_span_image_filter_rgb.h>
#include <agg_span_image_filter_rgba.h>
#include <agg_span_interpolator_linear.h>
#include <agg_renderer_scanline.h>
#include <agg_span_allocator.h>
#include <agg_rendering_buffer.h>
#include <agg_image_accessors.h>
/// Local files
#include "colorcode.h"
#include "CImg.h"

using namespace cimg_library;


int main(int argc, char** argv)
{
  if (argc < 2) {
    std::cout << "Usage: " << argv[0] << " <flow.pfm>" << std::endl;
    return EXIT_FAILURE;
  }
  const std::string flowfile{argv[1]};

  /// The optical flow input (first channel = x flow, second channel = y flow)
  CImg<float> flow(flowfile.c_str());


  const int W{flow.width()};
  const int H{flow.height()};

  /// Setup image output as CImg, and attach its data buffer to an AGG
  /// rendering object (to avoid having to copy the results around).
  unsigned char* scratch = new unsigned char[W * H * 3];
  CImg<unsigned char> out_cimg_view(scratch, 3, W, H, 1, true);
  agg::rendering_buffer rbuf(scratch, W, H, W*3);
  agg::pixfmt_rgb24 pixf(rbuf);
  agg::renderer_base<agg::pixfmt_rgb24> ren_base(pixf);

  /// Setup AGG render engine (scanline rasterizer with antialiasing)
  agg::scanline_u8 sl;
  agg::rasterizer_scanline_aa<> ras;
  agg::renderer_base<agg::pixfmt_rgb24> ren_sl(ren_base);


  /// Storage for a single flow vector
  agg::path_storage path;

  /// Randomness for flow vector dash style
  std::random_device randev;
  std::mt19937 mersenne(randev());
  std::uniform_real_distribution<> random_start_time(0, 1);
  std::vector<float> dash_offsets;

  /// Number of frames to render; the total flow is divided into so many steps
  const int FRAMES = 48;
  for (int frame_idx = 0; frame_idx < FRAMES; ++frame_idx) {
    /// Initialize visualized flow field to "white"
    ren_base.clear(agg::rgba8(255,255,255));

    /// Flow progress in [0,1)
    const float uniform_offset{(float)frame_idx/FRAMES};

    /// Flow visualization color (we use Middlebury style)
    unsigned char flow_colorcode_target[3];

    /// Density of rendered flow vectors (spacing in pixels)
    const int spacing{10};
    size_t linear_idx{0};
    /// Iterate over target image
    for (int yi = spacing/2; yi < H-spacing/2; yi+=spacing) {
      for (int xi = spacing/2; xi < W-spacing/2; xi+=spacing) {
        if (frame_idx == 0) {
          dash_offsets.push_back(random_start_time(mersenne));
        }
        const float x = xi;
        const float y = yi;
        const float x_to{x+flow(x,y,0)};
        const float y_to{y+flow(x,y,1)};
        /// Create flow vector 
        if (x == x_to and y == y_to) {
          path.remove_all();

          path.move_to(x, y);
          path.line_to(x+1e-2, y+1e-2);

          agg::conv_curve<agg::path_storage> curve(path);
          agg::conv_dash<agg::conv_curve<agg::path_storage>> dash(curve);
          dash.add_dash(1e-3, 4e-3);
          dash.dash_start(0.0);
          agg::conv_stroke<agg::conv_dash<agg::conv_curve<agg::path_storage>>> stroke(dash);
          stroke.width(2.0);
          stroke.line_cap(agg::round_cap);
          
          ras.reset();
          ras.add_path(stroke);
          agg::render_scanlines_aa_solid(ras, sl, ren_sl, agg::rgba8(255,255,255));
        } else {
          const float flow_mag{std::sqrt((x-x_to)*(x-x_to)+(y-y_to)*(y-y_to))};
          const float dash_length{flow_mag/10.f};

          path.remove_all();

          path.move_to(x, y);
          path.line_to(x_to, y_to);

          agg::conv_curve<agg::path_storage> curve(path);
          agg::conv_dash<agg::conv_curve<agg::path_storage>> dash(curve);
          /// Dashing style for flow vectors
          dash.add_dash(dash_length, 4*dash_length);
          dash.dash_start((2.f-(uniform_offset+dash_offsets[linear_idx]))*flow_mag*5/10);
          agg::conv_stroke<agg::conv_dash<agg::conv_curve<agg::path_storage>>> stroke(dash);
          stroke.width(2.0);
          stroke.line_cap(agg::round_cap);
          
          ras.reset();
          ras.add_path(stroke);
          computeColor(flow(x,y,0), flow(x,y,1), flow_colorcode_target);
          agg::render_scanlines_aa_solid(ras, sl, ren_sl, 
                                         agg::rgba8(flow_colorcode_target[0],
                                                    flow_colorcode_target[1],
                                                    flow_colorcode_target[2]));
        }
        ++linear_idx;
      }
    }  /// <-- loop over pixels

    /// Write out finished flow image
    {
      std::ostringstream oss;
      oss << "frame-" << std::setw(4) << std::setfill('0') << frame_idx << ".ppm";
      out_cimg_view.get_permute_axes("YZCX").save(oss.str().c_str());
    }
  }  /// <-- loop over frames

  
  /// Bye!
  return EXIT_SUCCESS;
}

