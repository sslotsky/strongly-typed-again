open Revery;
open Revery.Draw.Text;
open Revery.Math;
open Revery.UI;
open Revery.UI.Components;

type word = {
  text: string,
  velocity: int,
  top: int,
};

type gameState = {words: list(word)};

type action =
  | Drop;

let drop = (words: list(word)) =>
  words |> List.map(w => {...w, top: w.top + w.velocity});

let reducer = (action, state) => {
  switch (action) {
  | Drop => {words: state.words |> drop}
  };
};

let fallingWord = {
  let component = React.component("fallingWord");
  (~children as _: list(React.syntheticElement), ~word: word, ~left, ()) =>
    component(hooks => {
      let family = "Roboto-Regular.ttf";
      let size = 20;

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
        <Positioned top={word.top} left>
          <Container height=50 width={measurement.width + 10}>
            <Text style=wordStyle text={word.text} />
          </Container>
        </Positioned>,
      );
    });
};

let game = {
  let component = React.component("game");
  (~children as _: list(React.syntheticElement), ()) =>
    component(hooks => {
      let (state, dispatch, hooks) =
        Hooks.reducer(
          ~initialState={
            words: [
              {text: "Hey wassup dawg", velocity: 1, top: 0},
              {text: "nothin wassup with you", velocity: 2, top: 0},
            ],
          },
          reducer,
          hooks,
        );

      let hooks =
        Hooks.effect(
          OnMount,
          () => {
            let clear = Tick.interval(_ => dispatch(Drop), Seconds(0.016));
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
        state.words |> List.map(w => <fallingWord word=w left=10 />);
      (hooks, <View style=containerStyle> ...fallingWords </View>);
    });
};

let init = app => {
  let _ = Revery.Log.listen((_, msg) => print_endline("LOG: " ++ msg));

  let win = App.createWindow(app, "Welcome to Revery!");

  let element = <game />;

  let _ = UI.start(win, element);
  ();
};

App.start(init);