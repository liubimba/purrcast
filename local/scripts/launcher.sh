ARTIFACTS_DIR=../../artifacts

$ARTIFACTS_DIR/launcher-cpp/bin/purrcast --snapserver.config $ARTIFACTS_DIR/config/snapserver.conf --snapserver.path $ARTIFACTS_DIR/launcher-cpp/bin/snapserver --snapclient.path $ARTIFACTS_DIR/launcher-cpp/bin/snapclient --server.path $ARTIFACTS_DIR/backend-go/backend-go --server.port 8080 \
--server.static_dir $ARTIFACTS_DIR/web-react --monitor.port 3000
