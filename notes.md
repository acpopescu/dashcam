# Deep Learning others
Nice progress, transformers moving in to other areas, but more work is needed to support multi-dimensional input data (text is 1d.) Some nice things:
- Visual attention networks https://github.com/Visual-Attention-Network, https://syncedreview.com/2022/02/23/deepmind-podracer-tpu-based-rl-frameworks-deliver-exceptional-performance-at-low-cost-211/ 

# Rolling shutter correction
https://arxiv.org/pdf/2104.01601.pdf and https://github.com/zzh-tech/RSCD are SUPER promising wrsp to automatic rolling shutter correction and deblurring. They are doing a bunch of deep learning algos to estimate motion in the frame. Looks really similar to https://ieeexplore.ieee.org/document/9411701 and the end 2 end DL may be better than the classical method discussed below with the rolling shutter correction

I tested this and a 4k does not fit within my GPU memory (8 gigs :( ) this makes me sad. I will try with smaller videos :(

## Alternatives to this
As the paper provides a public datasheet with the global vs rolling shutter operation, there is always an opportunity to re-train the network with smaller parameter. Alternatively a distillation step (check open NNX) could work. 

Finally a novel re-implementation may be interesting, but that is an active area of . (https://github.com/Visual-Attention-Network, spiking Res Nets/autoencoders)

## Other techniques
Also a nice collection of distortion-correction techniques: https://github.com/subeeshvasu/Awesome-Image-Distortion-Correction



# Rolling shutter correction _AND_ video stabilization
THis is an improvement apparently on the idea that pixels can move within a row.
https://ieeexplore.ieee.org/document/9411701
"Then, different from other 2D rolling shutter removal methods that assume the pixels in the same row have a single intra-frame motion, we build a novel mesh-based intra-frame motion calculation model to cope with the depth variation in a mesh row and obtain more faithful estimation results. Finally, temporal and spatial motion constraints and an adaptive weight assignment strategy are considered together to generate the optimal warping transformations for different motion situations. "


# Visual Odometry
Rolling-shutter independence: https://arxiv.org/abs/1808.00558

# Camera calibration
https://github.com/alexvbogdan/DeepCalib

