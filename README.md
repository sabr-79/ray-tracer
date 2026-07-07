# Ray Tracer


**To-do**:
- Get a simple, general ray tracer working: baseline Check.
- Add BVH Check.
- Apple Optimizations: GCD, vDSP, Metal for M2
- Benchmark General vs Optimized

**Current Performance Stats:**
- Naive (w/o BVH): 777s.
- BVH: 25s. 

### Since the final scene render takes a long time with naive implementation, I will do future performance checks with the BVH as the baseline. 