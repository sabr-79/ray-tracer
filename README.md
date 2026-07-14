# Ray Tracer


**To-do**:
- Get a simple, general ray tracer working: baseline Check.
- Add BVH Check.
- Apple Optimizations: GCD -- used thread lib instead, SIMD, Metal (?)
- Benchmark General vs Optimized -- script is set up, look into recording cache hits/misses


**From benchmark.zsh, apple branch (using thread lib over gcd) vs main branch**:
- ===== Results =====
- Main average:      43.37s
- Parallel average:  12.63s
- Speedup:           3.43x
- Individual times:
  - main:      42.7052400112152 43.605740070343 43.7865800857544
  - parallel:  13.2632501125336 12.5142600536346 12.1059901714325

Main in this case has BVH, Parallel is the apple branch w/ parallelism.

**After revising BVH, adding adaptive sampling and russian roulette, and SIMD**
Scene uses 1200 width, 500 samples, 50 depth.
- ===== Results =====
- Main average:      750.34s
- Parallel average:  66.36s
- Speedup:           11.31x
- Individual times:
  - main:      750.340829849243
  - parallel:  66.3573398590088