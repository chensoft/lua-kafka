package = "kafka"
version = "1.8.2-1"
source = {
   url = "git://github.com/chensoft/lua-kafka",
   branch = "v1.8.2"
}
description = {
   summary = "Lua bindings for Apache Kafka librdkafka library",
   detailed = "",
   homepage = "https://github.com/chensoft/lua-kafka",
   license = "MIT"
}
dependencies = {
   "lua >= 5.3"
}
build = {
   type = "cmake",
   variables = {
     CMAKE_BUILD_TYPE = "Release",
     CMAKE_INSTALL_PREFIX = "$(PREFIX)"
   },
}