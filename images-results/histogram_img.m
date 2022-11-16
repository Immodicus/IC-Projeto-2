%----Original image
figure(1)
title('Codec - Original Image');
I1 = imread('../images/15.256.ppm');
imhist(I1);
e1 = entropy(I1); % entropy = 7.0244
%-----encode image
figure(2)
title('Codec - Encode Image');
I2 = imread('images-results/15.256_img_decode.ppm');
imhist(I2);
e2 = entropy(I2); % entropy = 7.0244

% The lossless image is demonstrated
