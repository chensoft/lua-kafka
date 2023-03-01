local settings = {["bootstrap.servers"] = "localhost:9092", ["group.id"] = "1", ["auto.offset.reset"] = "earliest"}
local consumer = require("kafka").consumer(settings)

consumer:subscribe("hello1", "hello2")

while true do
    local message = consumer:poll(1000)
    if message then
        print(string.format("recv message topic %s, partition %d, payload %s, key %s, offset %d", message.topic, message.partition, message.payload, message.key, message.offset))
    end
end