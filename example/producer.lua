local settings = {["bootstrap.servers"] = "localhost"}
local producer = require("kafka").producer(settings)

for i = 1, 100 do
    print("send message " .. i)
    producer:produce("hello1", "Hello World! " .. i)
    producer:produce("hello2", i .. " Hello World!")
end

producer:poll(5000)
producer:close()