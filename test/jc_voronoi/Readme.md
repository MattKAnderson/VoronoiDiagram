# Brief

A test script to time the [jc_voronoi]() implementation over many problem sizes and input sets at each problem size. 

# Note on robustness

It is recommended to limit the max problem size to 25,000 sites and repeats to 10. The implementation will hang indefinitely on some input scenarios. This has been observed for a problem size as small as 13000 (during a test of 50 repeats per size).

The test script here differs from that in the [jc_voronoi]() repo. There are 2 main differences impacting results/robustness between these test scripts. The test script here uses a different set of sites as input every repeat whereas the jc_vonoroi test script resets the prng with `srand(0)` every repeat which results in the same set of sites. The second difference is that this script will generate any floating point number within the bounds for site x, y values, whereas the jc_voronoi test script produces integers that get casted to floats.