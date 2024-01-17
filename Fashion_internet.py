#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Fri May  5 15:03:19 2023

@author: ceciliaagosta
"""

import torch
import torch.nn as nn
import torch.nn.functional as F
import torch.optim as optim
from torch.utils.data import DataLoader, SubsetRandomSampler
from torchvision import datasets, transforms

import numpy as np

import matplotlib.pyplot as plt

# Define a transform to normalize the data
# HINT: you have to transform the images to tensors and then normalize them to have zero mean
transform = transforms.Compose([
    transforms.ToTensor(),
    transforms.Normalize((0.2861,), (0.3530,))
])

# Download the training data
trainset = datasets.FashionMNIST(root='/Users/ceciliaagosta/Desktop/Università/III anno/Lab IAGI/Esercitazione 7/data', train=True, download=True, transform=transform)
print(trainset)

#We prepare 20% of the train set as a validation set
indices = list(range(len(trainset)))
np.random.shuffle(indices)
split = int(np.floor(0.2*len(trainset)))
print(split)
valid_sample = SubsetRandomSampler(indices[:split])
train_sample = SubsetRandomSampler(indices[split:])

#Load the training and validation data
train_loader = DataLoader(trainset, sampler=train_sample, batch_size=64)
valid_loader = DataLoader(trainset, sampler=valid_sample, batch_size=64)

# Download and load the test data
testset = datasets.FashionMNIST(root='/Users/ceciliaagosta/Desktop/Università/III anno/Lab IAGI/Esercitazione 7/data', train=False, download=True, transform=transform)
test_loader = DataLoader(testset, batch_size=64, shuffle=True)
print(testset)

#Now we create the model
class Classifier(nn.Module):
    def __init__(self):
        super().__init__()
        
        #We will have input(784), 3 hidden layers(256,128,64) and output(10)
        #Could have built a stack with nn.Sequential. May do so later to simplify the forward function
        self.linear1 = nn.Linear(784, 256)
        self.act_f1 = nn.ReLU()
        
        self.linear2 = nn.Linear(256, 128)
        self.act_f2 = nn.ReLU()
        
        self.linear3 = nn.Linear(128, 64)
        self.act_f3 = nn.ReLU()
        
        self.linear4 = nn.Linear(64, 10)
        
        #We define a 20% dropout. During training this will randomly zero some elements of the input tensor, which has proven to be an effective technique for regularization
        self.dropout = nn.Dropout(0.2)
        
    #Now we define the forward function for our network
    def forward(self,x):
        x = x.view(x.shape[0], -1)
        
        x = self.linear1(x)
        x = self.act_f1(x)
        x = self.dropout(x)
        
        x = self.linear2(x)
        x = self.act_f2(x)
        x = self.dropout(x)
        
        x = self.linear3(x)
        x = self.act_f3(x)
        x = self.dropout(x)
        
        x = self.linear4(x)
        
        return F.log_softmax(x, dim=1)
    

model = Classifier()

#We define the loss function (Negative Log Likelihood Loss) and the optimizer (Stochastic Gradient Descent)
criterion = nn.NLLLoss()
optimizer = optim.SGD(model.parameters(), lr=0.01)

#We define the training function
def train(epoch, log_interval=200):
    model.train()
    
    for batch_idx, (images, labels) in enumerate(train_loader):
        
        optimizer.zero_grad()
        
        preds = model(images)
        preds = preds.squeeze(dim=1)
        
        loss = criterion(preds, labels)
        loss.backward()
        
        optimizer.step()
        
        if batch_idx % log_interval == 0:
            print('Train Epoch: {} [{}/{} ({:.0f}%)]\tLoss: {:.6f}'.format(
                epoch, batch_idx * len(images), len(train_loader.sampler),
                100. * batch_idx / len(train_loader), loss.data.item()))


#We define the validation function
def validate(loss_vector, accuracy_vector):
    
    model.eval()
    correct_count, all_count = 0, 0
    loss_count = 0
    
    for images,labels in valid_loader:
      for i in range(len(labels)):
        img = images[i].view(1, 784)
        with torch.no_grad():
            pred = model(img)

        ps = torch.exp(pred)
        probab = list(ps[0])
        pred_label = probab.index(max(probab))
        true_label = labels[i]
        if(true_label == pred_label):
          correct_count += 1
        else:
          loss_count += 1
          
        all_count += 1

    val_loss = loss_count/all_count
    accuracy = correct_count/all_count * 100

    loss_vector.append(val_loss)
    accuracy_vector.append(accuracy)
    
    print('\nValidation set: Average loss: {:.4f}, Accuracy: {}/{} ({:.2f}%)\n'.format(
        val_loss, correct_count, all_count, accuracy))
    
        
epochs = 10

lossv, accv = [], []
for epoch in range(1, epochs + 1):
    train(epoch)
    validate(lossv, accv)
    
plt.figure(figsize=(5,3))
plt.plot(np.arange(1,epochs+1), lossv)
plt.title('validation loss')

plt.figure(figsize=(5,3))
plt.plot(np.arange(1,epochs+1), accv)
plt.title('validation accuracy')

#Obtain one batch of test images
dataiter = iter(test_loader)
images, labels = next(dataiter)

#Get sample outputs
output = model(images)
_, preds = torch.max(output,1)
#Prep images for display
images = images.numpy()

#Plot the images in the batch along with the predicted and true labels
fig = plt.figure(figsize=(25,4))
for idx in np.arange(20):
    ax = fig.add_subplot(2, 10, idx+1, xticks=[], yticks=[])
    ax.imshow(np.squeeze(images[idx]), cmap='gray')
    ax.set_title("{} ({})".format(str(preds[idx].item()), str(labels[idx].item())), color=("green" if preds[idx]==labels[idx] else "red"))



