# RayTracing

## Ideas to implement:
- Cohen-Sutherland algorithm for AABB intersection speedup
- Mesh generation from .OBJ files
- Ingo Wald's papers like [fast, dynamic bvh's](https://graphics.stanford.edu/~boulos/papers/togbvh.pdf) 
- Optimizing bvh tree generation
- Implementing a good metric collecting system or more like learn to use `gprof`
- procedural/progressive raytracing
    - once this is in place you may wanna look into making it an app and moving the camera with keys

## Ideas from In One Weekend closing:
- Make it physically accurate.   This will imply using spectra instead of RGB (I like just using a big array of wavelengths) and get something where you know the reflectances.   Popular is to get a [X-Rite MSCCC ColorChecker Classic](http://www.amazon.com/gp/product/B000JLO31C/ref=as_li_tl?ie=UTF8&camp=1789&creative=9325&creativeASIN=B000JLO31C&linkCode=as2&tag=inonwe09-20&linkId=CXVB5F4SE4HGNRHV) whose data is [available online](http://www.babelcolor.com/colorchecker-2.htm#CCP2_data).
- Make it good for generating animations.    Lots of movies use a ray traced system, and [Disney](http://www.disneyanimation.com/technology/innovations/hyperion), [Pixar](http://graphics.pixar.com/library/), and the [Solid Angle](https://www.solidangle.com/arnold/research/) teams have both disclosed a remarkable amount about their code.   Work on features and then efficiency.   I think you will be amazed how soon you can produce amazing images.
- Make it fast.   Here you can roll your own, or start using a commercial API.   To see exactly where that community is now, go to the [2016 HPG conference](http://eventegg.com/hpg-2016/).   Or backtrack in their previous papers.   They are a very open community and the papers go into much detail relative to many sub-fields in computer graphics.