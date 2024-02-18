~effectsBus = Bus.audio(s, 2); // 2 channels for stereo audio

(
SynthDef(\cool_synth, { |out = 0, freq = 440, mod = 1, im = 1, pan=0, fbk=1, sustain = 1|
	var jitter = LFNoise1.kr(10, 0.05, 0.95);
	// var env = Env.new([0, 1, 1, 0], [0.04, sustain, 0.1]).kr(2) * jitter;
	var env = Env.perc.kr();
	var localin = LocalIn.ar(1) * fbk;

	var modulator = SinOsc.ar(freq * mod, mul: im * env);
	var carrier = SinOsc.ar(freq + modulator + localin);

	var panout = Pan2.ar(carrier, pan);

	LocalOut.ar(carrier);
	Out.ar(out, panout * env);
}).add;

SynthDef(\effectBus, {
    |in, out = 0, delayTime = 0.2, feedback = 0.5, mix = 0.5|
    var input, delayed;
    input = In.ar(in, 2);
    delayed = DelayN.ar(input, 2, delayTime, feedback);

    Out.ar(out, Mix.new([input, delayed], mix));
}).add;
)


Synth(\cool_synth, [\freq, 440, \mod, 1.5, \im, 1])

Pbind(\instrument, \cool_synth, \midinote, Pseq([65, 67, 59], 1), \dur, Pseq([1, 1, 1], 1)).play

(
Synth(\cool_synth, [\out, ~effectsBus, \freq, 440, \mod, 1.5, \im, 1]);

)

Synth(\effectBus, [\in, ~effectsBus, \mix, 0.1, \delayTime, 0.1]);