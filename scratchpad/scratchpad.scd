~effectsBus = Bus.audio(s, 2); // 2 channels for stereo audio

(
var numBuffers = 4;
var bufferSize = 44100;
var buffers = Array.fill(numBuffers, { Buffer.alloc(s, bufferSize, 2) });
var snippetSizes = [2048, 4096, 8192, 16384];

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

SynthDef(\effectBus, { |in, out = 0|
	var chosenBuffer = buffers.choose;
	var snippetSize = snippetSizes.choose;
	var audioIn = In.ar(in, 2);
	var saveSnippet = WhiteNoise.ar(1) > 0.95;
    var snippet = LocalBuf(snippetSize);
	var filteredSnippet;

    RecordBuf.ar(audioIn, chosenBuffer, loop: 0, doneAction: 2, run: saveSnippet);
	filteredSnippet = PlayBuf.ar(2, chosenBuffer) * 0.3; // Example: apply a simple filter

    // Apply the processed audio buffer to the output
    Out.ar(out, audioIn + filteredSnippet);
}).add;
)


Synth(\cool_synth, [\freq, 440, \mod, 1.5, \im, 1])

Pbind(\instrument, \cool_synth, \midinote, Pseq([65, 67, 59], 1), \dur, Pseq([1, 1, 1], 1)).play

(
Synth(\cool_synth, [\out, ~effectsBus, \freq, 440, \mod, 1.5, \im, 1]);

)

Synth(\effectBus, [\in, ~effectsBus]);