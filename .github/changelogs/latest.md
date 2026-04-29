# Version: 2026.04.29124558

* [cf3707e](https://github.com/ford-jones/lazarus/commit/cf3707ec5d5ad6c7e5dbac13494e88bff9795ae2): Update readme.md
* [aeec7c4](https://github.com/ford-jones/lazarus/commit/aeec7c4b2eb196963131d40e6e23d1735246f485): [Changelog CI] Add Changelog for Version 2026.04.29124352
* [cc87d1e](https://github.com/ford-jones/lazarus/commit/cc87d1ed2fdd31777d4e9da339d36603441d4718): Docs: Updated example reference
* [1841a13](https://github.com/ford-jones/lazarus/commit/1841a137dfab868205906bfdf3a55ba070b3d2a0): Docs: Updated API reference
* [a792d80](https://github.com/ford-jones/lazarus/commit/a792d80d40efc96af40bf316f21345d426c63d64): Feat: Loads multiple obj assets from a single file
* [ab5211d](https://github.com/ford-jones/lazarus/commit/ab5211de750fefdcc4a4e0e54e577c5653cf51c8): Update: Sets ambient lighting as configuratin default
* [b921b40](https://github.com/ford-jones/lazarus/commit/b921b40f537c721a3afde64a623f8ea9ecb08b57): Feat: Ambient lighting
* [a4fcf55](https://github.com/ford-jones/lazarus/commit/a4fcf55a376009b298417711657150fd3d71f15a): Feat: Wireframe mode
* [a880350](https://github.com/ford-jones/lazarus/commit/a880350a1edcc006222b0202903eb59d3000fb60): Feat: Wireframe mode
* [7ce4bcf](https://github.com/ford-jones/lazarus/commit/7ce4bcf6fce21912a500c7cb40413a5fa018b75d): Update: Refactored camera configuration
* [5465094](https://github.com/ford-jones/lazarus/commit/5465094c16b7534db50c85668b69321deb604b01): Update: Clearly labeled wavefront identifiers
* [eacf63c](https://github.com/ford-jones/lazarus/commit/eacf63c2c009bc5e0939a1feb765792dac7c60e9): Clean: Preferences wavefront structs internally
* [2ae2364](https://github.com/ford-jones/lazarus/commit/2ae2364437dedd82dd46073de3f14be8a919912a): Clean: Refactored wavefront asset loader
* [8d7a187](https://github.com/ford-jones/lazarus/commit/8d7a1876ee26bac34f19f4b7fc37609f94706b8b): Clean: Tidy up code comments
* [7d7e796](https://github.com/ford-jones/lazarus/commit/7d7e796bb5dcf9336ff6928b66e2162b791c3445): Feat: Extended animation playback tools to include a play function
* [80e2f45](https://github.com/ford-jones/lazarus/commit/80e2f454f1fa6c499f5583a1020192ecd92349c9): Update: Refactoring of joint-matrix local transform computation
* [7459e8d](https://github.com/ford-jones/lazarus/commit/7459e8dbbad0be5a56495a42f430afccecd9f4e8): Clean: Tidy up unused members
* [c739245](https://github.com/ford-jones/lazarus/commit/c739245723d57c386bd3de0ec363b4493abbd124): Update: Improved error checking
* [0633d83](https://github.com/ford-jones/lazarus/commit/0633d8395278e870582fb4af22ba5b937ae450a7): Feat: Playsback various animations in scenes with multiple moving assets
* [e355dd9](https://github.com/ford-jones/lazarus/commit/e355dd9a580624ba79ee4cbdef2881a39939164e): Docs: Updated code comments
* [5db9793](https://github.com/ford-jones/lazarus/commit/5db9793ea7654d9dd3643460611538a4e7f4f18b): Feat: Introduces mechanisms for pausing animations and resetting pose positions
* [f5bd114](https://github.com/ford-jones/lazarus/commit/f5bd11401474bcc4aa2a156fa9f1fd357df0c61c): Feat: Introduces mechanism for toggling animations
* [155fd68](https://github.com/ford-jones/lazarus/commit/155fd68f728f2675c46ba14b2f514c921d0e1293): Fix: Supports loading mesh components which have multiple animations
* [5e2c180](https://github.com/ford-jones/lazarus/commit/5e2c180326d9908fb679bbac278037e96998ef70): Update: Implements spherical interpolation for rotations with generic interpolation for translation and scale transforms. Handles step animation and scopes time-delta polling to animated loads
* [212983e](https://github.com/ford-jones/lazarus/commit/212983e9d9978265cf1131662a976706082a642c): Fix: Corrects animation flag sizing
* [d1b3f3d](https://github.com/ford-jones/lazarus/commit/d1b3f3d67a730847d214141204bd09a20f8cf71f): Clean: ModelLoader / animation code clean and updated comments
* [1868dfb](https://github.com/ford-jones/lazarus/commit/1868dfb7bcd326caf7dcca08499f3dca33b5b5b1): Feat: Correctly renders the animations of glb assets
* [ddde9d9](https://github.com/ford-jones/lazarus/commit/ddde9d9886fa6d624b4d432ef0ff9f1cc8cdfb12): Feat: Accumulates delta time and performs linear interpolation on animation keyframe values
* [d2f208a](https://github.com/ford-jones/lazarus/commit/d2f208a273cff2ab285e093089ee7e0250fab753): Feat: Precomputes armature pose position for assets with animated rigs
* [0a42357](https://github.com/ford-jones/lazarus/commit/0a423577ac8b54888c5aa86333de75d6bf240e18): Feat: Includes skinning matrix computation in default shader for animated assets
* [792cc5f](https://github.com/ford-jones/lazarus/commit/792cc5f254587bdcf037fd24a260bb9600aeb3f3): Feat: Computes joint matrices accross armature and performs GPU upload
* [190235a](https://github.com/ford-jones/lazarus/commit/190235a81c0af8c6e3494592144e20ff2184d766): Feat: Aligns and uploads keyframe and timestep uniforms to GPU
* [8deed37](https://github.com/ford-jones/lazarus/commit/8deed37b4616a15d0fcd91a480821112cad4b05e): Fix: Corrections to joint/bone lookups, prefer joint-indices over node-indices
* [2529b63](https://github.com/ford-jones/lazarus/commit/2529b63677389959b54788d45e641ed70213f75e): Update: Buffers joints and weights to GPU
* [9c79cc8](https://github.com/ford-jones/lazarus/commit/9c79cc8c361a697aaabefc34c929d2124361c286): Update: Includes animation data in loader outputs
* [9b6be08](https://github.com/ford-jones/lazarus/commit/9b6be08d47f1b558dbaaa79d688b51417331b7fd): Clean: Moved file analysis tools to file loader toolkit
* [9f3b490](https://github.com/ford-jones/lazarus/commit/9f3b4904ba788f265710da97466f59613e4a0ea8): Feat: Parses glb animation data
* [296d863](https://github.com/ford-jones/lazarus/commit/296d8639a81ee43aa53ffde9f77bc381e90a4051): Feat: Parses glb skinning data
* [696f763](https://github.com/ford-jones/lazarus/commit/696f763a40d4fc66c90fc53a552e0b3850170ce4): Feat: Extracts scale data and bakes the transform into vbo on-load
* [f878e29](https://github.com/ford-jones/lazarus/commit/f878e29098d7b0e4e9c2fd37918581c53540b704): Update: Identify glb animation-info structs
* [4f093e2](https://github.com/ford-jones/lazarus/commit/4f093e291b39a2d013542ddcd041d94eca72579e): Fix: Clear layercount progression between mesh loads
* [ae6a9a5](https://github.com/ford-jones/lazarus/commit/ae6a9a5c752e4e55c3ad63d4e03c7dd5f1b2429d): Clean: Updated naming conventions for 3D assets
* [bdad992](https://github.com/ford-jones/lazarus/commit/bdad99274edc0d08dd61a74c9a5690bb0559cfb0): Fix: Ensure stencil-buffer id assignment occurs by reference
* [c095d85](https://github.com/ford-jones/lazarus/commit/c095d857f5c540cc39de7f8e558694170ef04a49): Feat: Extracts rotation data and bakes the transform into vbo on-load
* [051251a](https://github.com/ford-jones/lazarus/commit/051251aeb0ddc31765956fc0e72dd4e3b4feb647): Feat: Extracts translation data and bakes the transform into vbo on-load
* [292abf4](https://github.com/ford-jones/lazarus/commit/292abf46e93fc277a6485e9789603bc2416f2209): Fix: Resolved memory leak from failure to free instance-info alongside other vertex buffers
* [63358e2](https://github.com/ford-jones/lazarus/commit/63358e26b4a21c2bc77b247766ce25134586de7a): Fix: Corrections to texture handling for multi-mesh glb asset loads
* [b1b8333](https://github.com/ford-jones/lazarus/commit/b1b83336c019ade4ee46a3d5d78f19a9ca8591ac): Feat: Support loading multiple glb assets at once
* [b1d1055](https://github.com/ford-jones/lazarus/commit/b1d1055a6bb22169458ff944a4f1921187e7b801): Update: Improved support for glb file format, loading of addtional node properties
* [7eac867](https://github.com/ford-jones/lazarus/commit/7eac867c7faeab17e51d0dfc23e7ffd33c09d70c): Clean: Ensure glb properties are correctly scoped at the right depth when parsing
* [d7dfea0](https://github.com/ford-jones/lazarus/commit/d7dfea0a8802fce2d3640d4fe3bce0e5f2886bb5): Fix: Reveal raw time in debug logs
