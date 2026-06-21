#!/usr/bin/env python3
"""
Reconstruct grayscale height maps from tangent-space normal maps.

Sponza's *_ddn.tga files are 24-bit RGB normal maps with no height/displacement
channel, so parallax mapping has nothing to sample. This integrates the normal
field's slopes into a height field using the Frankot-Chellappa method (FFT based),
which assumes the texture is periodic -- exactly right for tiling materials.

Output: one single-channel (grayscale) *_height.tga next to each input, so the
engine loads it as GL_RED with GL_REPEAT wrapping (tiles correctly).

Usage:
    python gen_height_from_normals.py <textures_dir> [--flip-y] [--suffix _ddn:_height]
"""
import sys
import os
import glob
import argparse
import numpy as np
from PIL import Image


def reconstruct_height(normal_rgb, flip_y=False):
    """normal_rgb: HxWx3 uint8 -> height: HxW float in [0,1]."""
    n = normal_rgb.astype(np.float64) / 255.0 * 2.0 - 1.0  # decode to [-1,1]
    nx = n[..., 0]
    ny = n[..., 1]
    nz = n[..., 2]
    if flip_y:
        ny = -ny
    # avoid division by ~0 for near-flat normals
    nz = np.where(np.abs(nz) < 1e-3, 1e-3, nz)

    # surface slopes: dh/dx = -nx/nz, dh/dy = -ny/nz
    p = -nx / nz
    q = -ny / nz

    H, W = nz.shape
    # frequency grids (cycles), wrap-around (periodic) integration
    wx = np.fft.fftfreq(W) * 2.0 * np.pi
    wy = np.fft.fftfreq(H) * 2.0 * np.pi
    fx, fy = np.meshgrid(wx, wy)

    denom = fx**2 + fy**2
    denom[0, 0] = 1.0  # avoid div by zero at DC

    P = np.fft.fft2(p)
    Q = np.fft.fft2(q)

    Z = (-1j * fx * P - 1j * fy * Q) / denom
    Z[0, 0] = 0.0  # undefined DC term -> arbitrary offset, set 0

    height = np.real(np.fft.ifft2(Z))

    # normalize to [0,1]
    hmin, hmax = height.min(), height.max()
    if hmax - hmin < 1e-8:
        return np.zeros_like(height)
    return (height - hmin) / (hmax - hmin)


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("textures_dir")
    ap.add_argument("--flip-y", action="store_true",
                    help="flip green channel (DirectX-style normal maps)")
    ap.add_argument("--match", default="*_ddn.tga",
                    help="glob for input normal maps (default *_ddn.tga)")
    ap.add_argument("--from-token", default="_ddn",
                    help="token in input name to replace for output")
    ap.add_argument("--to-token", default="_height",
                    help="replacement token for output name")
    args = ap.parse_args()

    inputs = sorted(glob.glob(os.path.join(args.textures_dir, args.match)))
    if not inputs:
        print(f"no files matched {args.match} in {args.textures_dir}")
        return 1

    print(f"reconstructing height for {len(inputs)} normal maps...")
    for path in inputs:
        img = Image.open(path).convert("RGB")
        arr = np.asarray(img)
        height = reconstruct_height(arr, flip_y=args.flip_y)
        out_arr = np.clip(height * 255.0 + 0.5, 0, 255).astype(np.uint8)

        base = os.path.basename(path)
        out_name = base.replace(args.from_token, args.to_token)
        if out_name == base:  # token not present, append before extension
            stem, ext = os.path.splitext(base)
            out_name = stem + args.to_token + ext
        out_path = os.path.join(args.textures_dir, out_name)

        # save as single-channel grayscale TGA -> stb loads as 1 component -> GL_RED
        Image.fromarray(out_arr, mode="L").save(out_path)
        print(f"  {base} -> {out_name}")

    print("done.")
    return 0


if __name__ == "__main__":
    sys.exit(main())
