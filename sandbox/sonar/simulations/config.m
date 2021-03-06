% Basic design parameters %

c                  = 1500.0;    % speed of sound in water in m/s
f                  = 30e3;      % frequency of signal in Hertz
lambda             = c / f;     % wavelength of signal in water in meters
hydro_pos_accuracy = 0.0000635; % machining tolerance in meters
ping_duration      = 1.3e-3;    % ping duration in seconds
f_s                = 1e6;       % sample rate in samples per second
tdoa_accuracy      = lambda/150;% TDOA resolution in meters
depth_accuracy     = 2*lambda;  % depth sensor resolution in meters
adc_bits           = 8;
adc_amplitude      = 1;
signal_amplitude   = 0.5;
snr                = 10;
