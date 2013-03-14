#!/bin/sh

scons debug=1 profile=0 deploy=0
scons debug=0 profile=0 deploy=0
scons debug=0 profile=1 deploy=0
scons debug=0 profile=0 deploy=1
