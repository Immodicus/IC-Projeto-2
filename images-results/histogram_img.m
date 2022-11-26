%----Original image
figure(1)
title('Codec - Original Image');
I1 = imread('../images/15.256.ppm');
imhist(I1);
e1 = entropy(I1); % entropy = 7,0244
%-----encode image
figure(2)
title('Codec - decode Image');
I2 = imread('15.256_img_decode.ppm');
imhist(I2);
e2 = entropy(I2); % entropy = 7,0244
% The lossless image is demonstrated

%------ before image copy
figure(3)
title('Image - original');
I3 = imread('../images/girl.ppm');
imhist(I3);
e3 = entropy(I3); % entropy = 6,4165
%-----after image copy
figure(4)
title('Image - copy');
I4 = imread('girl_cpy.ppm');
imhist(I4);
e4 = entropy(I4); % entropy = 6,4165
%----------------------------

% compression ratio
%I5 = imread('15.256_img_encode.ppm');
%info_img = imfinfo('15.256_img_encode', 'fmt');
%[rows cols layers] = size(I1); % geting the 3 elements of original img 
%uncomp_file_size = (rows*colos*layers)/8; % in bytes

%[rows2 cols2 layers2] = size(I5); % geting the 3 elements of encoder image
%comp_file_size = (rows2*colos2*layers2)/8; % in bytes


