open Revery;
open Revery.Draw.Text;
open Revery.Math;
open Revery.UI;
open Revery.UI.Components;

let wordPool = ["Hey wassup dawg", "nothin wassup with you"];

type word = {
  text: string,
  velocity: int,
  top: int,
  left: int,
};

type gameConfig = {
  fontSize: int,
  fontFamily: string,
  height: int,
  width: int,
};

type gameState = {words: list(word)};

type action =
  | Drop
  | Spawn(gameConfig);

let drop = (words: list(word)) =>
  words |> List.map(w => {...w, top: w.top + w.velocity});

let rec spawn = (pool: list(string), cfg: gameConfig) => {
  let index = pool |> List.length |> Random.int;
  let word = index |> List.nth(pool);
  let {fontFamily, fontSize} = cfg;
  let measurement = measure(~fontFamily, ~fontSize, word);

  if (measurement.width <= cfg.width) {
    let maxLeft = cfg.width - measurement.width;
    let left = Random.int(maxLeft);
    print_endline("The word is " ++ word);
    print_endline("The window is " ++ string_of_int(cfg.width));
    print_endline("The word measures " ++ string_of_int(measurement.width));
    print_endline("The max left coordinate is " ++ string_of_int(maxLeft));
    print_endline("The chosen left coordinate is " ++ string_of_int(left));
    {text: word, velocity: Random.int(3) + 1, top: 0, left};
  } else {
    cfg |> spawn(pool |> List.filter(w => w != word));
  };
};

let reducer = (action, state) => {
  switch (action) {
  | Drop => {words: state.words |> drop}
  | Spawn(cfg) => {words: [cfg |> spawn(wordPool), ...state.words]}
  };
};

let fallingWord = {
  let component = React.component("fallingWord");
  (
    ~children as _: list(React.syntheticElement),
    ~word: word,
    ~fontSize,
    ~fontFamily,
    (),
  ) =>
    component(hooks => {
      let family = fontFamily;
      let size = fontSize;

      let wordStyle =
        Style.[
          color(Colors.white),
          fontFamily(family),
          fontSize(size),
          flexDirection(`Row),
        ];

      let measurement =
        measure(~fontFamily=family, ~fontSize=size, word.text);

      (
        hooks,
        <Positioned top={word.top} left={word.left}>
          <Container height={measurement.height} width={measurement.width + 1}>
            <Text style=wordStyle text={word.text} />
          </Container>
        </Positioned>,
      );
    });
};

let game = {
  let component = React.component("game");
  (~children as _: list(React.syntheticElement), ~height, ~width, ()) =>
    component(hooks => {
      let fontSize = 20;
      let fontFamily = "Roboto-Regular.ttf";

      let (state, dispatch, hooks) =
        Hooks.reducer(~initialState={words: []}, reducer, hooks);

      let hooks =
        Hooks.effect(
          OnMount,
          () => {
            let clear =
              Tick.interval(
                _ => {
                  dispatch(Drop);
                  if (Random.int(1800) <= 10) {
                    dispatch(Spawn({fontSize, fontFamily, height, width}));
                  };
                },
                Seconds(0.016),
              );
            Some(clear);
          },
          hooks,
        );

      let containerStyle =
        Style.[
          position(`Absolute),
          justifyContent(`Center),
          alignItems(`Center),
          bottom(0),
          top(0),
          left(0),
          right(0),
        ];

      let fallingWords =
        state.words
        |> List.map(w => <fallingWord word=w fontSize fontFamily />);
      (hooks, <View style=containerStyle> ...fallingWords </View>);
    });
};

let init = app => {
  let _ = Revery.Log.listen((_, msg) => print_endline("LOG: " ++ msg));

  let win = App.createWindow(app, "Welcome to Revery!");
  let height = win.metrics.size.height;
  let width = win.metrics.size.width;

  let element = <game height width />;

  let _ = UI.start(win, element);
  ();
};

App.start(init);