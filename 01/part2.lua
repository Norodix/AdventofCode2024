-- Count the number of occurences of a value in the table
function occurences(table, value)
	local occ = 0
	for i, v in ipairs(table) do
		if v == value then
			occ = occ + 1
		end
	end
	return occ
end

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
table.sort(a)
table.sort(b)
local sum = 0
for i, v in ipairs(a) do
	local sim_score = v * occurences(b, v)
	sum = sum + sim_score
end
print(sum)
