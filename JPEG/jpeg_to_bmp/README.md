# JPEG Decoder

## Overview

I will develop the first version in VS2017 Community Env, which makes me focus on algorithm.
Then optimize it to embeded system (see https://github.com/jinfeng-zhuang/zstack).

## Desgin Rules

1. DON'T use all-in-one decoder structure, make every function more independent
2. make stack & data independent, this impact the function parameters
3. remove malloc, use more static CONFIG