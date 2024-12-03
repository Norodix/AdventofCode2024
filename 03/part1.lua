local line = io.read()

local format = "mul%(%d+,%d+%)"
local sum = 0
while line ~= nil do
	for mul in string.gmatch(line, format) do
		local _, _, x, y = string.find(mul, "(%d+),(%d+)")
		sum = sum + x * y
	end
	line = io.read()
end
print(sum)
