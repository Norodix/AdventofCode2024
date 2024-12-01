local line = io.read()

local a = {}
local b = {}
while line ~= nil do
	local _, _, x, y = string.find(line, "(%d+)%s+(%d+)")
	a[#a + 1] = tonumber(x)
	b[#b + 1] = tonumber(y)
	line = io.read()
end
local inspect = require("inspect")
-- print(inspect(a))
-- print(inspect(b))
table.sort(a)
table.sort(b)
-- print(inspect(a))
-- print(inspect(b))
local diffsum = 0
for i, v in ipairs(a) do
	local diff = math.abs(a[i] - b[i])
	diffsum = diffsum + diff
end
print(diffsum)
