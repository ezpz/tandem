### Next Steps
- [ ] Move each display to it's own thread
- [ ] Locking mechanisms for multiple threads grabbing the shared display
- [ ] Rectangular selection highlights points
- [ ] Events processed using 0MQ
- [ ] Multiple views supported in corner plot (switch via command/meta-key)
- [ ] Plot titles (general annotations: xlabel, ylabel, etc.)
- [ ] Move dataset/routines (median, mean, ...) to src/data, include/data
- [ ] Top and left plots in unison
- [ ] Separate adding points and displaying (plot functions)
- [ ] Make cindent normalization script
- [ ] Collect aspect ratio of screen when creating plots
- [ ] Better tick mark placement algorithm
- [ ] splines instead of just points
- [ ] Better edge cases (e.g. plot with single point)
- [ ] Better categories in the TODO list (maybe with priorities)
- [ ] Devise a way to determine 'unset' vs. 'user specified' parameters
- [x] How to handle axes on hostogram plots
- [x] Histogram view doesnt include other views (only the histogram)
- [x] Dataset structure instead of passing around a vector (with Range, etc.)
- [x] General text drawing
- [x] Fix Axis labels
- [x] Modularize code and improve directory/file layout
- [x] Better build process (clean and use Makefile properly)
- [x] Handle negative axis ranges
- [x] Read in dataset from command line (dynamically setting ranges)

### Types of views in corner display
- [ ] Median with std dev circles around selected points
- [ ] Correlation
- [ ] Line plot (line-and-point plot)
- [ ] cumulative distribution plot
- [ ] heatmap (standard binning)
- [ ] heatmap (various binning - hexagon, circle, ...)
- [ ] Histogram with color indicating quartiles
- [x] Histogram (x-axis/y-axis option)
- [x] boxplot

### Thoughts or nice-to-haves
- [ ] Regression/unit test support; integrated with build
- [ ] Dotted lines for grid
- [ ] Dynamically resize plot area
- [ ] Command line interface (i.e. 'language')
- [x] Non-scatterplot views in upper/lower plots
