open Revery;
open Revery.Draw.Text;
open Revery.Math;
open Revery.UI;
open Revery.UI.Components;

type action =
  | Drop(int);

type state = {top: int};

let reducer = (action, state) => {
  switch (action) {
  | Drop(velocity) => {top: state.top + velocity}
  };
};

let fallingWord = {
  let component = React.component("fallingWord");
  (~children as _: list(React.syntheticElement), ~text, ~velocity, ~left, ()) =>
    component(hooks => {
      let (state, dispatch, slots) =
        Hooks.reducer(~initialState={top: 0}, reducer, hooks);

      let hooks =
        Hooks.effect(
          OnMount,
          () => {
            let clear =
              Tick.interval(_ => dispatch(Drop(velocity)), Seconds(0.05));
            Some(clear);
          },
          slots,
        );

      let family = "Roboto-Regular.ttf";
      let size = 20;

      let wordStyle =
        Style.[
          color(Colors.white),
          fontFamily(family),
          fontSize(size),
          flexDirection(`Row),
        ];

      let measurement = measure(~fontFamily=family, ~fontSize=size, text);

      (
        hooks,
        <Positioned top={state.top} left>
          <Container height=50 width={measurement.width + 10}>
            <Text style=wordStyle text />
          </Container>
        </Positioned>,
      );
    });
};

let init = app => {
  let _ = Revery.Log.listen((_, msg) => print_endline("LOG: " ++ msg));

  let win = App.createWindow(app, "Welcome to Revery!");

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

  let element =
    <View style=containerStyle>
      <fallingWord text="hey wassup dawg how ya doin" velocity=2 left=10 />
    </View>;

  let _ = UI.start(win, element);
  ();
};

App.start(init);