[y,Fs]=wavread('gong.wav');
sound(y,Fs);
M = ones(size(y)-300,300);
%stanov� velikost matice
for t=300:size(y)
for i=1:299
M(t-299,i+1)=y(t-i);
end
end
%napln� matici
x=M\y(300:size(y));
%vy�e�� opt. �lohu
ytylda =(zeros(1,size(y)-300));
%pro n�sl. algorytmus je podstatn� aby ytylda byla 0v� vektor
 for i=1:67396
    for j=1:300
        ytylda(i) =ytylda(i) +(M(i,j)*x(j));
    end
end
hold on
plot(301:1:size(y),y(301:size(y)), 'r')
plot(301:1:size(y),ytylda, 'b')
xlabel('vzorek');
ylabel('hodnota vzorku');
title('prav� gong/syntetick� gong');
hold off
%v p��loze pos�l�m graf synt. gongu posunut� o 1 proto�e jinak se dokonale
%p�ekryj�
sound(y,Fs)
sound(ytylda,Fs)
%omlouv�m se mo�n� jsem se n�kde upsal kdy� jsem n�co p�episoval ale vy�lo
%mi �e sound(y,Fs) a sound(ytylda-1,Fs) jsou nerozeznateln� uchem ... tak�e
%v�e nasv�d�uje tomu �e je v�e v po��dku