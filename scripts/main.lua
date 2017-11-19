require "core"

A = class(function(self, value)
    self.value = value
end)

function A:getValue()
    return self.value
end

function A:setValue(value)
    self.value = value
end

local a = A(123)

io.write(a:getValue(),"\n")
a:setValue(77)
io.write(a:getValue(),"\n")