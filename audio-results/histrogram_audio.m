%----Original audio - 1st. order
%figure(1)
[audioIn,fs] = audioread("../audio/sample04.wav");
%entropy = spectralEntropy(audioIn,fs);
%plot(entropy);
%spectralEntropy(audioIn,fs)
%p1 = sum(audioIn)./length(audioIn);
%e1 = -sum(p1.*log2(p1));
%----
p1 = hist(audioIn);
p1(p1==0) = [];
p1 = p1./numel(audioIn);
e1 =  -sum(p1.*log2(p1)); % [1.4803,1.5028] for channel 1 and 2
tiledlayout(2,1)
nexttile
plot(e1)
title('Sinal da entropia do áudio original')
%----Audio decoding - 1st. order
%figure(2)
nexttile
[audioIn2,fs2] = audioread("../audio-results/sample04_dec_1er.wav");
%entropy2 = spectralEntropy(audioIn2,fs2);
%plot(entropy2);
%spectralEntropy(audioIn2,fs2)
p2 = sum(audioIn2)./length(audioIn2);
e2 = -sum(p2.*log2(p2)); % [1.4803,1.5028] for channel 1 and 2
%-----
p2 = hist(audioIn2);
p2(p2==0) = [];
p2 = p2./numel(audioIn2);
e2 =  -sum(p2.*log2(p2));
plot(e2)
title('Sinal  da entropia após descodificação do áudio')
%--- entropy of two channel
e12 = e1(1)+e1(2); % 2.9830

%------------------------------------------------------------
%Audio decoding - 2nd. order
[audioIn3,fs3] = audioread("../audio-results/sample04_dec_2er.wav");
p3 = hist(audioIn3);
p3(p3==0) = [];
p3 = p3./numel(audioIn3);
e3 =  -sum(p1.*log2(p1));
%------------------------------------------------------------
%Audio decoding - 3rd. order
[audioIn4,fs4] = audioread("../audio-results/sample04_dec_3er.wav");
p4 = hist(audioIn4);
p4(p4==0) = [];
p4 = p4./numel(audioIn4);
e4 =  -sum(p4.*log2(p4));
%------------------------------------------------------------
%Audio decoding - interChannel
[audioIn5,fs5] = audioread("../audio-results/sample04_dec_interChannel.wav");
p5 = hist(audioIn5);
p5(p5==0) = [];
p5 = p5./numel(audioIn5);
e5 =  -sum(p5.*log2(p5));

%Audio decoding - 1st.order - 8bit of lossy 
figure(2)
[audioIn7,fs7] = audioread("../audio/sample04.wav");
p7 = hist(audioIn7);
p7(p7==0) = [];
p7 = p7./numel(audioIn7);
e7 =  -sum(p7.*log2(p7)); % [1.4803,1.5028] for channel 1 and 2
tiledlayout(2,1)
nexttile
plot(e7)
title('Sinal da entropia do áudio original')
[audioIn6,fs6] = audioread("../audio-results/sample04_dec_1er_lossy_8bit.wav");
p6 = hist(audioIn6);
p6(p6==0) = [];
p6 = p6./numel(audioIn6);
e6 =  -sum(p6.*log2(p6)); % [1.4693,1.4919] for channel 1 and 2
e6_12 = e6(1)+e6(2); %2.9611
nexttile
plot(e6);
title('Sinal  da entropia após descodificação do áudio - perdas 8bits')

%-----------compression ration
[audioIn8,fs5] = audioread("../audio-results/sample04_enc_1er.wav");
ratio = audioIn/audioIn8;