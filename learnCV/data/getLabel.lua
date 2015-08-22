require 'torch'
require 'nn'
require 'image'
local modelName = "/Users/kingo/workshop/learnTorch/holdem/results/model.net"
local model = torch.load(modelName)
function getLabel(imgName)
	img = image.load(imgName)
	pre = model:forward(img[1])
	idx=1
	maxVal = pre[1]
	for i=2,(#pre)[1] do if(pre[i] > maxVal) then maxVal = pre[i]; idx=i end end
	return idx
end
--local imgName = '/Users/kingo/workshop/learnTorch/holdem/out0.jpg'
--print(getLabel(imgName))
