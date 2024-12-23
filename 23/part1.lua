local inspect = require("inspect")
local line = io.read()

local computers = {}
while line ~= nil do
	local _, _, x, y = string.find(line, "([a-z]+)-([a-z]+)")
	if not computers[x] then
		computers[x] = {}
	end
	computers[x][#computers[x] + 1] = y
	if not computers[y] then
		computers[y] = {}
	end
	computers[y][#computers[y] + 1] = x
	line = io.read()
end

print(inspect(computers))

function table_has(table, value)
	for _, v in ipairs(table) do
		if value == v then
			return true
		end
	end
	return false
end

local triplets = {}

for k, v in pairs(computers) do
	-- k = aq
	-- v = {yn, vc}
	for _, v1 in ipairs(v) do
		-- v1 = yn
		for _, v2 in ipairs(computers[v1]) do
			-- computers[v1] = {aq, cg, wh, td}
			-- v2 = cg
			if table_has(computers[v2], k) then
				-- table.append(triplets, { k, v1, v2 })
				local new = { k, v1, v2 }
				table.sort(new)
				local key = table.concat(new, ",")
				triplets[key] = new
			end
		end
	end
end

print(inspect(triplets))

local triplets_T = {}
for k, v in pairs(triplets) do
	for _, el in ipairs(v) do
		if string.sub(el, 1, 1) == "t" then
			triplets_T[k] = v
		end
	end
end

local count = 0
for _ in pairs(triplets_T) do
	count = count + 1
end

print(inspect(triplets_T))
print("Number of T triplets: " .. tostring(count))
