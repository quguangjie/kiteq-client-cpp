#!/bin/bash
protoc -I=./schema/ --cpp_out=./generated/ ./schema/kite_remoting.proto
