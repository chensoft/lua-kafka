# lua-kafka

Lua bindings for Apache Kafka librdkafka library

## Prepare

#### Debian/Ubuntu

```shell
apt install librdkafka-dev
```

#### MacOS

```shell
brew install librdkafka
```

#### Other OS

see https://github.com/edenhill/librdkafka

## Install

build with cmake or run `luarocks install kafka`

## Kafka

* producer(app_conf)
* consumer(app_conf)

## Producer

* produce(topic, payload)
* poll(timeout_ms)
* flush(timeout_ms)
* close()

```lua
local settings = {["bootstrap.servers"] = "localhost:9092"}
local producer = require("kafka").producer(settings)

for i = 1, 100 do
    print("send message " .. i)
    producer:produce("hello1", "Hello World! " .. i)
    producer:produce("hello2", i .. " Hello World!")
end

producer:poll(5000)
producer:close()
```

## Consumer

* subscribe(...)  // topics
* poll(timeout_ms)
* close()

```lua
local settings = {["bootstrap.servers"] = "localhost:9092", ["group.id"] = "1", ["auto.offset.reset"] = "earliest"}
local consumer = require("kafka").consumer(settings)

consumer:subscribe("hello1", "hello2")

while true do
    local message = consumer:poll(1000)
    if message then
        print(string.format("recv message topic %s, partition %d, payload %s, key %s, offset %d", message.topic, message.partition, message.payload, message.key, message.offset))
    end
end
```

## License

lua-kafka is released under the MIT License. See the LICENSE file for more information.

## Contact

You can contact me by email: admin@chensoft.com.